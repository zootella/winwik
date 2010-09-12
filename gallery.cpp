
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

galleryitem::galleryitem()
{
	// POINTERS
	next = NULL;
	back = NULL;
}

void Gallery()
{
	// takes nothing
	// displays the gallery dialog box
	// returns nothing

	// DISPLAY THE GALLERY DIALOG BOX
	DialogBox(Handle.instance, "DB_GALLERY", Handle.window, (DLGPROC)DialogGallery);
}

BOOL CALLBACK DialogGallery(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {

	// THE DIALOG IS ABOUT TO BE DISPLAYED
	case WM_INITDIALOG: {

		// LOAD TEXT FROM THE REGISTRY INTO THE DIALOG BOX
		string file, name;
		RegistryReadText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Gallery", "file", &file);
		RegistryReadText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Gallery", "name", &name);
		SetDlgItemText(dialog, IDC_FILE, file);
		SetDlgItemText(dialog, IDC_NAME, name);

		// LET THE SYSTEM PLACE THE FOCUS
		return(true); }

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		// THE USER CLICKED A BROWSE BUTTON
		case IDC_FILEBROWSE:
		case IDC_NAMEBROWSE: {

			// USE THE BROWSE FOR FOLDER DIALOG BOX TO GET A FOLDER PATH
			string path;
			if (LOWORD(wParam) == IDC_FILEBROWSE) path = FileChoose();
			if (LOWORD(wParam) == IDC_NAMEBROWSE) path = FileBrowse("Read image file names from this folder.");

			// WRITE THE FOLDER PATH INTO THE DIALOG BOX
			if (is(path) && LOWORD(wParam) == IDC_FILEBROWSE) SetDlgItemText(dialog, IDC_FILE, path);
			if (is(path) && LOWORD(wParam) == IDC_NAMEBROWSE) SetDlgItemText(dialog, IDC_NAME, path);
			break; }

		// THE USER CLICKED OK, CANCEL, OR THE CLOSE X
		case IDOK:
		case IDCANCEL: {

			// DISABLE CONTROLS
			EnableWindow(GetDlgItem(dialog, IDC_FILE),       false);
			EnableWindow(GetDlgItem(dialog, IDC_FILEBROWSE), false);
			EnableWindow(GetDlgItem(dialog, IDC_NAME),       false);
			EnableWindow(GetDlgItem(dialog, IDC_NAMEBROWSE), false);
			EnableWindow(GetDlgItem(dialog, IDC_EACH),       false);
			EnableWindow(GetDlgItem(dialog, IDOK),           false);
			EnableWindow(GetDlgItem(dialog, IDCANCEL),       false);

			// GET THE EACH SUBFOLDER OPTION
			bool each;
			if (IsDlgButtonChecked(dialog, IDC_EACH)) each = true;
			else                                      each = false;

			// STORE TEXT FROM THE DIALOG BOX INTO THE REGISTRY
			string file2, name2;
			file2 = TextDialog(dialog, IDC_FILE);
			name2 = TextDialog(dialog, IDC_NAME);
			RegistryWriteText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Gallery", "file", file2);
			RegistryWriteText(HKEY_CURRENT_USER, "Software\\Rocky Mountain Tools\\Gallery", "name", name2);

			// IF THE USER CLICKED OK, PERFORM THE GALLERY OPERATION
			if (LOWORD(wParam) == IDOK) GalleryStart(file2, name2, each);

			// CLOSE THE DIALOG
			EndDialog(dialog, 0);
			return(true); }
		}

		break;
	}

	return(false);
}

void GalleryStart(read file, read name, bool each)
{
	// takes text
	// performs the generate gallery operation
	// returns nothing

	// CHECK THE PATHS
	if (FileLook(file) <= 0)  { MessageBox(Handle.window, "Unable to find the template file.",          "Rocky Mountain Tools", MB_OK | MB_ICONEXCLAMATION); return; }
	if (FileLook(name) != -1) { MessageBox(Handle.window, "Unable to find the folder of named images.", "Rocky Mountain Tools", MB_OK | MB_ICONEXCLAMATION); return; }

	// GENERATE EACH SUBFOLDER
	string done;
	if (each) {

		// GET A LIST OF THE FOLDERS BEFORE YOU ADD THE DONE FOLDER
		string subfolders, subfolder;
		subfolders = ListFolders(on(name, "\\", Reverse, Different));

		// COMPOSE THE PATH TO THE SAVE FOLDER
		done = on(name, "\\", Reverse, Different) + "(done)";
		if (FileLook(done) != -2 || !FileFolder(done, 0)) { MessageBox(Handle.window, "Done folder may already exist.", "Rocky Mountain Tools", MB_OK | MB_ICONEXCLAMATION); return; }

		// LOOP TO GENERATE EACH SUBFOLDER
		while (is(subfolders)) { split(subfolders, "\n", &subfolder, &subfolders);

			// GENERATE A GALLERY
			GalleryFolder(file, subfolder, done);
		}

	// GENERATE THIS ONE FOLDER
	} else {

		// COMPOSE THE PATH TO THE DONE FOLDER
		done = before(off(name, "\\", Reverse, Different), "\\", Reverse, Different) + "\\(done)";
		if (FileLook(done) != -2 || !FileFolder(done, 0)) { MessageBox(Handle.window, "Done folder may already exist.", "Rocky Mountain Tools", MB_OK | MB_ICONEXCLAMATION); return; }

		// GENERATE A GALLERY
		GalleryFolder(file, name, done);
	}
}

void GalleryFolder(read file, read name, read done)
{
	// takes text
	// performs the generate gallery operation
	// returns nothing

	// COMPOSE AND MAKE THE SAVE FOLDER FOR THIS GALLERY
	string save;
	save = after(off(name, "\\", Reverse, Different), "\\", Reverse, Different);
	save = on(done, "\\", Reverse, Different) + save + "\\";
	FileFolder(save, 0);

	// GET FILE NAMES FROM THE FOLDER AND ITS SUBFOLDERS TO BUILD THE SORTED GALLERY LIST
	HANDLE search, searchfolder;
	string found, foundfolder, foundname, foundext, foundgroup;
	search = NULL;
	while (FileList(on(name, "\\", Reverse, Different), "*.*", &search, &found)) {

		// FILE
		if (!trails(found, "\\")) {

			found = after(found, "\\", Reverse, Different);
			split(found, ".", &foundname, &foundext, Reverse, Different);
			foundgroup = "";
			GalleryAdd(foundname, foundext, foundgroup);

		// FOLDER
		} else {

			searchfolder = NULL;
			while(FileList(found, "*.*", &searchfolder, &foundfolder)) {

				// FILE IN A FOLDER
				if (!trails(foundfolder, "\\")) {

					split(foundfolder, "\\", &foundfolder, &foundname, Reverse, Different);
					split(foundname, ".", &foundname, &foundext, Reverse, Different);
					foundgroup = after(foundfolder, "\\", Reverse, Different);
					GalleryAdd(foundname, foundext, foundgroup);
				}
			}
		}
	}

	// GET THE TEXT FROM THE TEMPLATE FILE
	string plate;
	plate = GalleryFileRead(file);

	// COMPOSE TITLE TEXT LIKE "series title" AND FOLDER TEXT LIKE "series_title"
	string title, folder;
	title = after(off(name, "\\", Reverse, Different), "\\", Reverse, Different);
	folder = replace(title, " ", "_");

	// GENERATE THE GALLERY PAGES
	GalleryGenerate(save, plate, title, folder);

	// DELETE THE GALLERY LIST
	while (State.g) {

		// THE LIST HAS MORE THAN ONE ITEM
		if (State.g->next) {

			State.g = State.g->next;
			delete State.g->back;
			State.g->back = NULL;

		// THE LIST HAS JUST ONE ITEM
		} else {

			delete State.g;
			State.g = NULL;
		}
	}
}

void GalleryAdd(read name, read ext, read group)
{
	// takes text
	// makes a new gallery item and adds it to the list in the correct sorted position
	// returns nothing

	// MAKE A NEW GALLERY ITEM AND SET ITS TEXT
	galleryitem *g = new galleryitem;
	g->name  = name;
	g->ext   = ext;
	g->group = group;

	// THIS IS THE FIRST ITEM FOR THE LIST
	if (!State.g) {

		State.g = g;
		return;
	}

	// MOVE I DOWN THE GALLERY LIST
	galleryitem *i;
	i = State.g;
	while (i) {

		// I IS AT THE TOP OF THE LIST
		if (!i->back) {

			// G SHOULD GO BEFORE I, ADD IT TO THE TOP OF THE LIST
			if (g->name < i->name) {

				i->back = g;
				g->next = i;
				State.g = g;
				return;

			// G SHOULD GO AFTER I, WHICH IS THE ONLY ITEM IN THE LIST
			} else if (!i->next) {

				i->next = g;
				g->back = i;
				return;
			}

		// I IS IN THE MIDDLE OF THE LIST
		} else if (i->next) {

			// G SHOULD GO BEFORE I, ADD IT ABOVE I HERE IN THE LIST
			if (g->name < i->name) {

				i->back->next = g;
				g->back = i->back;
				i->back = g;
				g->next = i;
				return;
			}

		// I IS AT THE END OF THE LIST
		} else {

			// ADD I TO THE END OF THE LIST
			i->next = g;
			g->back = i;
			return;
		}

		i = i->next;
	}
}

string GalleryFileRead(read path)
{
	// takes path to a text file
	// opens it and reads all its text into a string
	// returns the string, blank if no text or any error

	// OPEN THE FILE
	DWORD size;
	HANDLE file, map;
	LPVOID view;
	size = FileReadOpen(path, &file, &map, &view);
	if (!size) return(blank);

	// OPEN A STRING
	string s;
	write buffer = s.GetBuffer(size);
	buffer[size] = '\0';

	// COPY THE CHARACTERS
	lstrcpyn(buffer, (read)view, size + 1);

	// CLOSE THE STRING
	s.ReleaseBuffer();

	// CLOSE THE FILE
	FileReadClose(file, map, view);

	// RETURN THE STRING
	return(s);
}

bool GalleryFileWrite(read path, read r)
{
	// takes a path and text
	// creates and opens a new file at the path and writes in all the text
	// returns true if successful, false otherwise

	// ELIMINATE EMPTY LINES IN THE FILE, LINES AT THE START, AND SET ONE AT THE END
	string s = r;
	while (has(s, "\x0D\x0A\x0D\x0A")) s = replace(s, "\x0D\x0A\x0D\x0A", "\x0D\x0A");
	s = trim(s, "\x0D\x0A") + "\x0D\x0A";

	// CHECK THE FOLDER
	bool result;
	result = FileFolder(path, 1); // PATH INCLUDES A FILE NAME
	if (!result) return(false);

	// MAKE SURE THE PATH IS AVAILIABLE
	if (FileLook(path) != -2) return(false);

	// OPEN A NEW FILE FOR WRITING
	HANDLE file;
	file = FileWriteOpen(path, 0);
	if (!file) return(false);

	// WRITE THE TEXT
	result = FileWriteText(file, s);
	if (!result) { FileWriteClose(file); return(false); }

	// CLOSE THE FILE AND REPORT SUCCESS
	result = FileWriteClose(file);
	if (!result) return(false);
	return(true);
}

void GalleryGenerate(read save, read plate, read title, read folder)
{
	// takes a folder path with a trailing backslash to save the files in, and template, title, and folder text
	// uses thes information and the list of gallery items to generate and save the gallery pages
	// returns nothing

	// GET THE GROUP AND VIEW TEMPLATES
	string groupplate, group, viewplate, view, s;
	groupplate = parse(plate, "[group/]", "[/group]");
	viewplate = parse(plate, "[view/]", "[/view]");

	// MOVE G DOWN EACH ITEM WITH GROUP TEXT
	galleryitem *g, *h;
	g = State.g;
	while (g) {

		if (is(g->group)) {

			// MOVE H DOWN EACH ITEM WITH THE SAME GROUP TEXT AS G
			s = g->group;
			h = g;
			while(h) {

				if (same(s, h->group)) {

					// ADD THIS VIEW AND CLEAR THE ITEM'S GROUP TEXT
					view += replace(replace(viewplate, "[name]", h->name), "[ext]", h->ext);
					h->group = "";
				}

				h = h->next;
			}

			// ADD THIS GROUP AND BLANK THE VIEW TEXT BEFORE STARTING THE NEXT ONE
			group += replace(groupplate, "[view]", view);
			view = "";
		}

		g = g->next;
	}

	// COMPOSE THE SELECTED PAGE
	string selected;
	selected = parse(plate, "[selected/]", "[/selected]");
	selected = replace(selected, "[group]", group);
	selected = replace(selected, "[title]", title);
	selected = replace(selected, "[folder]", folder);

	// SAVE THE SELECTED PAGE
	GalleryFileWrite(make(save, "index.html"), selected);

	// GET THE PAGE AND IMAGE TEMPLATES
	string pageplate, imageplate;
	pageplate = parse(plate, "[page/]", "[/page]");
	imageplate = parse(plate, "[image/]", "[/image]");

	// VARIABLES TO COMPOSE AND COUNT PAGES
	string page, pages, left, right;
	int n;

	// LOAD THE FIRST PAGE TEMPLATE AND START OUT THAT 0 PAGES HAVE BEEN FINISHED
	page = pageplate;
	n = 0;

	// MOVE G DOWN THE LIST OF GALLERY ITEMS
	g = State.g;
	while (g) {

		// OUT OF SPACES
		if (!has(page, "[image]")) {

			// ADD THE FINISHED PAGE TO PAGES AND RESET THE PAGE TEMPLATE
			pages += page + "[page]";
			n++;
			page = pageplate;
		}

		// LOAD ONE IMAGE INTO THE PAGE
		split(page, "[image]", &left, &right);
		page = left + replace(replace(imageplate, "[name]", g->name), "[ext]", g->ext) + right;

		g = g->next;
	}

	// DONE WITH ITEMS, ELIMINATE UNUSED IMAGE SPACES AND ADD THE LAST PAGE TO PAGES
	page = replace(page, "[image]", "");
	pages += page + "[page]";
	n++;

	// WRITE THE TITLE AND FOLDER FOR ALL THE PAGES
	pages = replace(pages, "[title]", title);
	pages = replace(pages, "[folder]", folder);

	// LOOP I FROM 1 THROUGH THE LAST PAGE NUMBER
	int i;
	for (i = 1; i <= n; i++) {

		// BREAK ONE PAGE OFF THE FRONT OF PAGES
		split(pages, "[page]", &page, &pages);

		// WRITE IN THE PAGES PREVIOUS, NEXT, AND CONTENTS LINKS
		page = replace(page, "[previous]", GalleryPrevious(i, n));
		page = replace(page, "[next]",     GalleryNext(i, n));
		page = replace(page, "[contents]", GalleryContents(i, n, 20));

		// SAVE THE PAGE TO DISK
		GalleryFileWrite(save + numerals(i) + ".html", page);
	}
}

string GalleryPrevious(int page, int total)
{
	// takes a page number, the total number of pages, and html template text for the link
	// generates html for the previous link
	// returns a string

	// COMPOSE THE PREVIOUS LINK FOR ALL BUT THE FIRST PAGE
	if (page == 1) return(blank);
	else           return("<a href=\"" + numerals(page - 1) + ".html\">&lt; Previous Page</a>");
}

string GalleryNext(int page, int total)
{
	// takes a page number, the total number of pages, and html template text for the link
	// generates html for the next link
	// returns a string

	// COMPOSE THE PREVIOUS LINK FOR ALL BUT THE LAST PAGE
	if (page == total) return(blank);
	else               return("<a href=\"" + numerals(page + 1) + ".html\">Next Page &gt;</a>");
}

string GalleryContents(int onpage, int pages, int size)
{
	// takes a page number, the total number of pages, and the number of pages to break into a group
	// generates html for the page contents links
	// returns a string

	// COMPOSE THE HTML
	int page, first, last;
	string s;
	for (page = 1; page <= pages; page++) {

		// CALCULATE THE FIRST AND LAST PAGE NUMBERS IN THE GROUP
		first = last = page;
		while (first % size != 1) first--;
		while (last  % size != 0) last++;

		// COMBINE THE LAST 2 GROUPS IF THE LAST ONE IS NOT FULL
		if (last > pages) {

			// THIS PAGE IS IN THE LAST GROUP, WHICH IS NOT FULL
			first -= size;
			if (first < 1) first = 1;
			last = pages;

		} else if (last + size > pages) {

			// THIS PAGE IS IN THE GROUP BEFORE THE LAST GROUP, WHICH WILL NOT BE FULL
			last = pages;
		}

		// INSIDE
		if (first <= onpage && onpage <= last) {

			// HAVE A LINE BREAK BEFORE THE INSIDE PAGES
			if (page == first) s += "<br>\x0D\x0A";

			// ADD LINE TELLING THE PAGE NUMBER OR LINKING TO IT
			if (page == onpage) s += "On Page " + numerals(page) + "<br>\x0D\x0A";
			else s += "<a href=\"" + numerals(page) + ".html\">Page " + numerals(page) + "</a><br>\x0D\x0A";

			// HAVE A LINE BREAK AFTER THE INSIDE PAGES
			if (page == last) s += "<br>\x0D\x0A";

		// OUTSIDE, BUT FIRST PAGE IN GROUP
		} else if (page == first) {

			// ADD LINE LINKING TO THE GROUP OF PAGES
			s += "<a href=\"" + numerals(first) + ".html\">Pages " + numerals(first) + " - " + numerals(last) + "</a><br>\x0D\x0A";
		}
	}

	// TRIM BREAKS FROM EDGES
	return(trim(s, "<br>\x0D\x0A"));
}
