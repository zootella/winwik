
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

void Backup()
{
	// takes nothing
	// displays the backup dialog box
	// returns nothing

	// DISPLAY THE BACKUP DIALOG BOX
	DialogBox(Handle.instance, "DB_BACKUP", Handle.window, (DLGPROC)DialogBackup);
}

BOOL CALLBACK DialogBackup(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	// THE DIALOG IS ABOUT TO BE DISPLAYED
	switch (message) {
	case WM_INITDIALOG: {

		// SAVE THE DIALOG HANDLE AND INITIALIZE ALL THE STATE VARIABLES
		Handle.dialog = dialog;
		BackupInitialize();

		// SEND THE LIMIT TEXT MESSAGE WITH 0 AS THE LIMIT TO HAVE WINDOWS DRASTICIALLY EXPAND THE TEXT LIMIT
		SendMessage(GetDlgItem(dialog, IDC_COMMANDS), EM_LIMITTEXT, 0, 0);
		SendMessage(GetDlgItem(dialog, IDC_ERRORS),   EM_LIMITTEXT, 0, 0);

		// LOAD THE COMMANDS TEXT FROM THE REGISTRY
		string s1;
		RegistryReadText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Backup", "commands", &s1);
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, s1);

		// LET THE SYSTEM PLACE THE FOCUS
		return(true);

	// THE DIALOG NEEDS TO BE PAINTED
	break; }
	case WM_PAINT: {

		// DO CUSTOM PAINTING IN THE DIALOG
		deviceitem device;
		device.OpenPaint(dialog);
		BackupPaint(&device);

	// THE MESSAGE IS A COMMAND
	break; }
	case WM_COMMAND:

		// THE USER CLICKED AN EDIT BUTTON
		switch (LOWORD(wParam)) {
		case IDC_CLEAR:
		case IDC_DELETE:
		case IDC_COPY:
		case IDC_UPDATE:
		case IDC_COMPARE:
		case IDC_BROWSE:

			// EDIT THE COMMANDS TEXT
			BackupEdit(LOWORD(wParam));

		// THE USER CLICKED START
		break;
		case IDC_START:

			// PERFORM THE BACKUP OPERATION
			BackupStart();

		// THE USER CLICKED THE CLOSE X
		break;
		case IDCANCEL:

			// SAVE THE COMMANDS TEXT INTO THE REGISTRY
			string s2;
			s2 = TextDialog(Handle.dialog, IDC_COMMANDS);
			RegistryWriteText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Backup", "commands", s2);

			// CLEAR THE DIALOG HANDLE AND RESET ALL THE STATE VARIABLES
			Handle.dialog = NULL;
			BackupInitialize();

			// CLOSE THE DIALOG
			EndDialog(dialog, 0);
			return(true);

		break;
		}

	break;
	}

	return(false);
}

void BackupPaint(deviceitem *device)
{
	// takes a device item
	// paints the stage text in the backup dialog box
	// returns nothing

	// PAINT THE STAGE TEXT
	sizeitem size;
	size.w = 800;
	size.y = -15;
	size.SetBottom(52);
	if      (State.backup.stage == BackupStageStart)          PaintText(device, "start",            size, false, true, false, false, 71, Handle.arial, &Handle.lightblue,   &Handle.blue);
	else if (State.backup.stage == BackupStageRunning)        PaintText(device, "running",          size, false, true, false, false, 69, Handle.arial, &Handle.lightyellow, &Handle.yellow);
	else if (State.backup.stage == BackupStageDone)           PaintText(device, "done",             size, false, true, false, false, 72, Handle.arial, &Handle.lightgreen,  &Handle.green);
	else if (State.backup.stage == BackupStageDoneWithErrors) PaintText(device, "done with errors", size, false, true, false, false, 72, Handle.arial, &Handle.lightred,    &Handle.red);
}

void BackupInitialize()
{
	// takes nothing
	// initializes all the state variables used in a backup operation
	// returns nothing

	// BEGIN STAGE
	State.backup.stage = BackupStageStart;

	// BLANK STRINGS
	State.backup.errors = "";

	// ZERO COUNTS
	State.backup.folder       = 0;
	State.backup.foldererror  = 0;
	State.backup.file         = 0;
	State.backup.fileerror    = 0;
	State.backup.compare      = 0;
	State.backup.compareerror = 0;

	// NO IGNORE
	State.backup.ignore = false;
}

void BackupEdit(WORD button)
{
	// takes the identifier of the clear, delete, copy, compare, or browse button
	// shows browse dialog boxes and edits the contents of the commands text box
	// returns nothing

	// GET THE TEXT FROM THE DIALOG BOX
	string commands, source, destination;
	commands = TextDialog(Handle.dialog, IDC_COMMANDS);

	// CLEAR
	if (button == IDC_CLEAR) {

		SetDlgItemText(Handle.dialog, IDC_COMMANDS, "");

	// DELETE
	} else if (button == IDC_DELETE) {

		source = FileBrowse("Choose the folder to delete.");
		commands += make("\x0D\x0A", "delete•", source, "••");
		commands = trim(commands, "\x0D\x0A");
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, commands);

	// COPY
	} else if (button == IDC_COPY) {

		source = FileBrowse("Choose the folder to copy.");
		destination = FileBrowse("Choose the destination folder for the copy.");
		commands += make("\x0D\x0A", "copy•", source, "•", destination, "•");
		commands = trim(commands, "\x0D\x0A");
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, commands);

	// UPDATE
	} else if (button == IDC_UPDATE) {

		source = FileBrowse("Choose the source folder.");
		destination = FileBrowse("Choose the destination folder for the update.");
		commands += make("\x0D\x0A", "update•", source, "•", destination, "•");
		commands = trim(commands, "\x0D\x0A");
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, commands);

	// COMPARE
	} else if (button == IDC_COMPARE) {

		source = FileBrowse("Choose the folder to compare.");
		destination = FileBrowse("Choose the destination folder for the compare.");
		commands += make("\x0D\x0A", "compare•", source, "•", destination, "•");
		commands = trim(commands, "\x0D\x0A");
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, commands);

	// BROWSE
	} else if (button == IDC_BROWSE) {

		source = FileBrowse("Choose a folder to insert its path.");
		commands += "\x0D\x0A" + source;
		commands = trim(commands, "\x0D\x0A");
		SetDlgItemText(Handle.dialog, IDC_COMMANDS, commands);
	}
}

void BackupStart()
{
	// takes nothing
	// performs the backup operations
	// returns nothing

	// DISABLE BUTTONS
	EnableWindow(GetDlgItem(Handle.dialog, IDC_IGNORE),  false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_CLEAR),   false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_DELETE),  false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_COPY),    false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_UPDATE),  false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_COMPARE), false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_BROWSE),  false);
	EnableWindow(GetDlgItem(Handle.dialog, IDC_START),   false);

	// GET THE COMMANDS TEXT FROM THE DIALOG BOX AND REMOVE NEWLINE CHARACTERS
	string commands;
	commands = TextDialog(Handle.dialog, IDC_COMMANDS);
	commands = replace(commands, "\x0D", "");
	commands = replace(commands, "\x0A", "");

	// GET THE IGNORE OPTION
	if (IsDlgButtonChecked(Handle.dialog, IDC_IGNORE)) State.backup.ignore = true;

	// SET THE STAGE TO RUNNING
	State.backup.stage = BackupStageRunning;
	deviceitem device;
	device.OpenGet(Handle.dialog);
	BackupPaint(&device);

	// RECORD THE STARTING TICK
	DWORD time;
	time = GetTickCount();

	// PERFORM COMMANDS
	BackupCommands(commands);

	// DISPLAY HOW LONG THE OPERATIONS TOOK
	string s;
	s = TextTime(GetTickCount() - time);
	BackupDisplayTask(s);

	// SET THE STAGE TO DONE OR DONE WITH ERRORS
	if (is(State.backup.errors)) State.backup.stage = BackupStageDoneWithErrors;
	else                         State.backup.stage = BackupStageDone;
	BackupPaint(&device);
}

void BackupCommands(read r)
{
	// takes the commands text
	// parses it and performs each command
	// returns nothing

	// MAKE STRINGS AND COPY IN THE COMMANDS TEXT
	string s, command, source, destination, display;
	s = r;

	// SET A FLAG TO NOT DELETE ANYTHING AFTER AN ERROR
	bool safetodelete;
	safetodelete = true;

	// LOOP WHILE S HAS TEXT
	while (is(s)) {

		// READ THE COMMAND, SOURCE, AND DESTINATION TEXT
		split(s, "•", &command,     &s);
		split(s, "•", &source,      &s);
		split(s, "•", &destination, &s);

		// MAKE PATHS HAVE TRAILING BACKSLASHES
		if (is(source)) source = on(source, "\\", Reverse, Different);
		if (is(destination)) destination = on(destination, "\\", Reverse, Different);

		// COMPOSE THE DISPLAY ERROR TEXT
		display = "Unable to " + command + "•" + source + "•" + destination + "•";

		// COPY
		if (has(command, "copy", Matching)) {

			if (FileFolder(source, 0) && FileFolder(destination, 0)) { BackupCopy(source, destination, source); }
			else                                                     { BackupDisplayError(display); safetodelete = false; }

		// UPDATE
		} else if (has(command, "update", Matching)) {

			if (safetodelete && FileFolder(source, 0) && FileFolder(destination, 0)) { BackupUpdate(source, destination, false); } // Don't compare
			else                                                                     { BackupDisplayError(display); safetodelete = false; }

		// COMPARE UPDATE
		} else if (has(command, "compdate", Matching)) {

			if (safetodelete && FileFolder(source, 0) && FileFolder(destination, 0)) { BackupUpdate(source, destination, true); } // Compare
			else                                                                     { BackupDisplayError(display); safetodelete = false; }

		// COMPARE
		} else if (has(command, "compare", Matching)) {

			if (FileFolder(source, 0) && FileFolder(destination, 0)) { BackupCompare(source, destination, source); }
			else                                                     { BackupDisplayError(display); safetodelete = false; }

		// DELETE
		} else if (has(command, "delete", Matching)) {

			if (safetodelete && isblank(destination) && FileFolder(source, 0)) { BackupDelete(source); }
			else                                                               { BackupDisplayError(display); safetodelete = false; }

		// INVALID COMMAND
		} else {

			BackupDisplayError(display);
			safetodelete = false;
		}
	}
}

void BackupCopy(read source, read destination, read folder)
{
	// takes the source and destination folders of the operation and a folder path to copy
	// copies all the contents of the folder across, updating status and errors
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string from, to, display;
	bool result;
	while (FileList(folder, "*.*", &find, &from)) {

		// USE SOURCE, DESTINATION, AND FROM TO COMPOSE TO
		to = destination + clip(from, length(source), -1);

		// FROM IS A FOLDER
		if (trails(from, "\\")) {

			// COMPOSE TASK TEXT
			display = "Foldering •" + from + "• to •" + to + "•";
			BackupDisplayTask(display);

			// PERFORM THE TASK
			result = FileFolder(to, 0);

			// SUCCESS, INCRIMENT THE COUNT OF FOLDERS
			if (result) {

				State.backup.folder++;
				BackupDisplayStatus();

			// ERROR, INCRIMENT THE COUNT OF FOLDERS ERRORS AND ADD THE TASK TO THE ERROR LIST
			} else {

				State.backup.foldererror++;
				BackupDisplayStatus();
				BackupDisplayError(display);
			}

			// MOVE INTO THE FOLDER WITH THIS FUNCTION
			BackupCopy(source, destination, from);

		// FROM IS A FILE
		} else {

			// COMPOSE TASK TEXT
			display = "Copying •" + from + "• to •" + to + "•";
			BackupDisplayTask(display);

			// PERFORM THE TASK
			result = FileMoveSimple(from, to, 0, 1); // DON'T FOLDER, COPY FILE

			// SUCCESS, INCRIMENT THE COUNT OF FILES
			if (result) {

				State.backup.file++;
				BackupDisplayStatus();

			// ERROR, INCRIMENT THE COUNT OF FILES ERRORS AND ADD THE TASK TO THE ERROR LIST
			} else {

				State.backup.fileerror++;
				BackupDisplayStatus();
				BackupDisplayError(display);
			}

			// IGNORE IS ON AND THIS FILE IS THE BACKUP IGNORE MARKER
			if (State.backup.ignore && trails(from, "\\!backup", Matching)) {

				// RECORD THE IGNORE IN ERRORS, CLOSE THE FIND, AND LEAVE THE FUNCTION
				BackupDisplayError(make("Copy ignored •", from, "•"));
				FindClose(find);
				return;
			}
		}
	}
}

void BackupCompare(read source, read destination, read folder)
{
	// takes the source and destination folders of the operation and a folder path to compare
	// compares all the contents of the folder across, updating status and errors
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string from, to, display;
	bool result;
	while (FileList(folder, "*.*", &find, &from)) {

		// USE SOURCE, DESTINATION, AND FROM TO COMPOSE TO
		to = destination + clip(from, length(source), -1);

		// FROM IS A FOLDER
		if (trails(from, "\\")) {

			// PERFORM THE TASK
			if (FileLook(from) != -1) BackupDisplayError("Checking source folder •"      + from + "•");
			if (FileLook(to)   != -1) BackupDisplayError("Checking destination folder •" + to   + "•");

			// MOVE INTO THE FOLDER WITH THIS FUNCTION
			BackupCompare(source, destination, from);

		// FROM IS A FILE
		} else {

			// COMPOSE TASK TEXT
			display = "Comparing •" + from + "• to •" + to + "•";
			BackupDisplayTask(display);

			// PERFORM THE TASK
			result = FileSame(from, to);

			// SUCCESS, INCRIMENT THE COUNT
			if (result) {

				State.backup.compare++;
				BackupDisplayStatus();

			// ERROR, INCRIMENT THE COUNT AND ADD THE TASK TO THE ERROR LIST
			} else {

				State.backup.compareerror++;
				BackupDisplayStatus();
				BackupDisplayError(display);
			}

			// IGNORE IS ON AND THIS FILE IS THE BACKUP IGNORE MARKER
			if (State.backup.ignore && trails(from, "\\!backup", Matching)) {

				// RECORD THE IGNORE IN ERRORS, CLOSE THE FIND, AND LEAVE THE FUNCTION
				BackupDisplayError(make("Compare ignored •", from, "•"));
				FindClose(find);
				return;
			}
		}
	}
}

void BackupDelete(read folder)
{
	// takes a path to a folder that ends with a trailing slash
	// deletes the contents and then the folder
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string path, display;
	bool result;
	while (FileList(folder, "*.*", &find, &path)) {

		// PATH IS A FOLDER
		if (trails(path, "\\")) {

			// MOVE INTO IT WITH THIS FUNCTION
			BackupDelete(path);

		// PATH IS A FILE
		} else {

			// DELETE IT
			display = "Deleting •" + path + "•";
			BackupDisplayTask(display);
			result = FileDelete(path);
			if (!result) BackupDisplayError(display);
		}
	}

	// DELETE THE FOLDER
	path = folder;
	display = "Deleting •" + path + "•";
	BackupDisplayTask(display);
	path = off(path, "\\", Reverse, Different);
	result = FileDelete(path);
	if (!result) BackupDisplayError(display);
}

void BackupDisplayTask(read display)
{
	// takes text that will only be read
	// displays it in the dialog box
	// returns nothing

	// SET THE TEXT IN THE DIALOG
	SetDlgItemText(Handle.dialog, IDC_TASK, display);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}

void BackupDisplayStatus()
{
	// takes nothing
	// composes and displays status text based on the state variables
	// returns nothing

	// COMPOSE THE DISPLAY TEXT
	string display;
	display = "Processed " +
		saynumber(State.backup.folder,       "folder")       + ", " +
		saynumber(State.backup.foldererror,  "folder error") + ", " +
		saynumber(State.backup.file,         "file")         + ", " +
		saynumber(State.backup.fileerror,    "file error")   + ", " +
		saynumber(State.backup.compare,      "file compare") + ", " +
		saynumber(State.backup.compareerror, "file compare error");

	// SET THE TEXT IN THE DIALOG
	SetDlgItemText(Handle.dialog, IDC_STATUS, display);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}

void BackupDisplayError(read display)
{
	// takes text that will only be read
	// appends it to the error list and updates the error list in the dialog box
	// returns nothing

	// APPEND THE DISPLAY TO THE ERRORS TEXT
	State.backup.errors += make(display, "\x0D\x0A");

	// SET THE TEXT IN THE DIALOG
	SetDlgItemText(Handle.dialog, IDC_ERRORS, State.backup.errors);

	// LET THE WINDOW AND DIALOG BOX REDRAW
	DialogUpdate();
}
