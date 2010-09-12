
// INCLUDE STATEMENTS
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// GLOBAL VARIABLES
extern handletop Handle; // GLOBAL HANDLE STRUCTURE
extern statetop  State;  // STATE VARIABLES

void Rename()
{
	// takes nothing
	// displays the rename dialog box
	// returns nothing

	// DISPLAY THE RENAME DIALOG BOX
	DialogBox(Handle.instance, "DB_RENAME", Handle.window, (DLGPROC)DialogRename);
}

BOOL CALLBACK DialogRename(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {

	// THE DIALOG IS ABOUT TO BE DISPLAYED
	case WM_INITDIALOG:

		// SAVE THE DIALOG HANDLE AND INITIALIZE ALL THE STATE VARIABLES
		Handle.dialog = dialog;
		RenameInitialize();

		// CHECK THE ONLY IMAGES CHECK BOX AND THE MIDDLE DUPLICATE OPTION
		//CheckDlgButton(dialog, IDC_ONLYIMAGES, BST_CHECKED);
		CheckRadioButton(dialog, IDC_DUPLICATE0, IDC_DUPLICATE2, IDC_DUPLICATE2);

		// SEND THE LIMIT TEXT MESSAGE WITH 0 AS THE LIMIT TO HAVE WINDOWS DRASTICIALLY EXPAND THE TEXT LIMIT
		SendMessage(GetDlgItem(dialog, IDC_ERRORS), EM_LIMITTEXT, 0, 0);

		// LET THE SYSTEM PLACE THE FOCUS
		return(true);

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		// THE USER CLICKED BROWSE
		case IDC_BROWSE: {

			string path;
			path = FileBrowse("Choose the folder to rename.");
			if (is(path)) SetDlgItemText(dialog, IDC_FOLDER, path);
			break; }

		// THE USER CLICKED START
		case IDC_START:

			// GET OPTIONS
			if      (IsDlgButtonChecked(dialog, IDC_ONLYIMAGES)) State.rename.onlyimages = true;
			else                                                 State.rename.onlyimages = false;
			if      (IsDlgButtonChecked(dialog, IDC_DUPLICATE0)) State.rename.duplicate = 0;
			else if (IsDlgButtonChecked(dialog, IDC_DUPLICATE1)) State.rename.duplicate = 1;
			else                                                 State.rename.duplicate = 2;
			if      (IsDlgButtonChecked(dialog, IDC_EACH))       State.rename.each = true;
			else                                                 State.rename.each = false;

			// PERFORM THE RENAME OPERATION
			RenameStart();
			break;

		// THE USER CLICKED THE CLOSE X
		case IDCANCEL:

			// CLEAR THE DIALOG HANDLE AND RESET ALL THE STATE VARIABLES
			Handle.dialog = NULL;
			RenameInitialize();

			// CLOSE THE DIALOG
			EndDialog(dialog, 0);
			return(true);
		}

		break;
	}

	return(false);
}

void RenameInitialize()
{
	// takes nothing
	// initializes all the state variables used in a rename operation
	// returns nothing

	// BLANK STRINGS
	State.rename.folder = "";
	State.rename.done = "";
	State.rename.errors = "";

	// RESET OPERATION VARIABLES
	State.rename.id = 0;

	// ZERO COUNTS
	State.rename.prepared = 0;
	State.rename.skipped = 0;
	State.rename.renamed = 0;
	State.rename.separated = 0;

	// ERASE THE LIST
	renameitem *r;
	State.rename.r = RenameFirst(0);
	while (State.rename.r) {

		r = State.rename.r;
		State.rename.r = State.rename.r->next[0];
		delete r;
	}
}

void RenameStart()
{
	// takes nothing
	// performs the rename operations on one folder or each subfolder
	// returns nothing

	// DISABLE CONTROLS
	EnableWindow(GetDlgItem(Handle.dialog, IDC_FOLDER),     false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_BROWSE),     false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_ONLYIMAGES), false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_DUPLICATE0), false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_DUPLICATE1), false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_DUPLICATE2), false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_START),      false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_EACH),       false);

	// RENAME EACH FOLDER
	if (State.rename.each) {

		// GET THE FOLDER PATH FROM THE DIALOG BOX
		string folder, subfolders, subfolder;
		folder = TextDialog(Handle.dialog, IDC_FOLDER);
		if (isblank(folder)) return;
		folder = on(folder, "\\", Reverse, Different);

		// LOOP FOR EACH SUBFOLDER PATH
		subfolders = ListFolders(folder);
		while (is(subfolders)) { split(subfolders, "\n", &subfolder, &subfolders);

			// RESET OPERATION VARIABLES
			State.rename.id = 0;

			// ERASE THE LIST
			renameitem *r;
			State.rename.r = RenameFirst(0);
			while (State.rename.r) {

				r = State.rename.r;
				State.rename.r = State.rename.r->next[0];
				delete r;
			}

			// RENAME THE SUBFOLDER
			State.rename.folder = subfolder;
			RenameFolder();
		}

	// RENAME JUST ONE FOLDER
	} else {

		// GET THE FOLDER PATH FROM THE DIALOG BOX, AND RENAME IT
		State.rename.folder = TextDialog(Handle.dialog, IDC_FOLDER);
		RenameFolder();
	}
}

void RenameFolder()
{
	// takes nothing
	// performs the rename operations
	// returns nothing

	// LOAD FOLDER PATH, MAKE THEM HAVE TRAILING BACKSLASHES, AND EDIT THEIR DISPLAY IN THE DIALOG BOX
	if (is(State.rename.folder)) State.rename.folder = on(State.rename.folder, "\\", Reverse, Different);
	SetDlgItemText(Handle.dialog, IDC_FOLDER, State.rename.folder);

	// COMPOSE DONE PATH
	State.rename.done = off(State.rename.folder, "\\", Reverse, Different) + "(done)\\";

	// CHECK THE FOLDER PATH DIDN'T MAKE THE DONE PATH TOO LONG
	if (length(State.rename.done) + length("(separated)/-folder---/(-number---)-name----------32---------------.-ext-----------32---------------") + SAFETY + SAFETY > MAX_PATH) {

		RenameDisplayError("Folder path too long");
		return;
	}

	// CHECK THE FOLDER EXISTS, THE DONE SUBFOLDER PATH IS AVAILIABLE, AND MAKE THE DONE SUBFOLDER
	if (RenameFileLook(State.rename.folder, NULL) != -1) { RenameDisplayError("Folder not found");               return; }
	if (RenameFileLook(State.rename.done, NULL)   != -2) { RenameDisplayError("Folder already has done folder"); return; }
	if (!FileFolder(State.rename.done, 0))               { RenameDisplayError("Cannot make done folder");        return; }

	// RECORD THE STARTING TICK
	DWORD time;
	time = GetTickCount();

	// MOVE ALL THE FILES TO BE RENAMED FROM THE SOURCE TREE INTO THE MIDDLE AREA
	RenameList(State.rename.folder);

	// MARK DUPLICATES, ASSIGN NUMBERS, AND GROUP INTO 90 MB FOLDERS
	if      (State.rename.duplicate == 1) RenameDuplicateNameAndData();
	else if (State.rename.duplicate == 2) RenameDuplicateData();
	RenameNumber();
	RenameGroup();

	// MOVE ALL THE FILES TO BE RENAMED FROM THE MIDDLE AREA TO THE DATED FOLDERS
	RenameMove();

	// DISPLAY HOW LONG THE OPERATIONS TOOK
	BackupDisplayTask(TextTime(GetTickCount() - time));
}

void RenameDisplayTask(read display)
{
	// takes text that will only be read
	// displays it in the dialog box
	// returns nothing

	// SET THE TEXT IN THE DIALOG
	SetDlgItemText(Handle.dialog, IDC_TASK, display);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}

void RenameDisplayStatus()
{
	// takes nothing
	// composes and displays status text based on the state variables
	// returns nothing

	// COMPOSE THE DISPLAY TEXT
	string display;
	display =
		saynumber(State.rename.prepared, "file")  + " prepared, " +
		saynumber(State.rename.skipped, "file")   + " skipped, "  +
		saynumber(State.rename.renamed, "file")   + " renamed, "  +
		saynumber(State.rename.separated, "file") + " separated";

	// SET THE TEXT IN THE DIALOG
	SetDlgItemText(Handle.dialog, IDC_STATUS, display);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}

void RenameDisplayError(read display)
{
	// takes text
	// appends it to the error list and updates the error list in the dialog box
	// returns nothing

	// APPEND THE DISPLAY TO THE ERRORS TEXT
	State.rename.errors += make(display, "\x0D\x0A");

	// SET THE TEXT IN THE DIALOG AND SCROLL DOWN ONE LINE
	SetDlgItemText(Handle.dialog, IDC_ERRORS, State.rename.errors);
	SendMessage(GetDlgItem(Handle.dialog, IDC_ERRORS), EM_SCROLL, SB_LINEDOWN, 0);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}
