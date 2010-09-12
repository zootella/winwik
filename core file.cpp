
// Include headers
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// Global objects
extern handletop Handle;

string FileMove(read path, read folder, read name, read ext, int action, int force, int attributes)
{
	// takes source path and destination text, action 0 move 1 copy, force 0 overwrite 1 avoid 2 use duplicate, attributes 0 no change 1 make visible
	// moves or copies the file, deleting files and making folders as requested and necessary
	// returns the destination path if successful, blank if any error

	// CHECK THE SOURCE FILE AND DESTINATION FOLDER
	if (FileLook(path) < 0 || !FileFolder(folder, 0)) return(blank);

	// LOOP FOR EACH NUMBERED DESTINATON PATH
	int n, look, result2;
	n = 1;
	string unique, destination;
	bool result;
	while (1) {

		// COMPOSE UNIQUE TEXT AND INCRIMENT NUMBER FOR THE NEXT LOOP
		if (n == 1) unique = "";
		else        unique = " (" + numerals(n) + ")";
		n++;

		// COMPOSE DESTINATION PATH
		destination = off(folder, "\\", Reverse, Different) + make("\\", name, unique, ext);

		// LOOK AT THE DESTINATION PATH
		look = FileLook(destination);
		if (look == -3) return(blank);

		// THERE IS A FILE ALREADY AT THE DESTINATION PATH, THE OPTION IS TO USE A DUPLICATE, AND THE FILE IS A DUPLICATE
		if (look >= 0 && force == 2 && FileSame(path, destination)) {

			// OPTION TO MOVE
			if (action == 0) {

				// DELETE THE SOURCE FILE
				result = FileDelete(path);
				if (!result) return(blank);
			}

			// EXIT LOOP
			break;
		}

		// THERE IS A FILE OR FOLDER ALREADY AT THE DESTINATON PATH AND THE OPTION IS TO OVERWRITE
		if (look >= -1 && force == 0) {

			// DELETE THE DESTINATION FILE OR FOLDER
			result = FileDelete(destination);
			if (!result) return(blank);
		}

		// THE FILE OR FOLDER AT THE DESTINATON PATH WAS JUST DELETED OR NEVER THERE
		if ((look >= -1 && force == 0) || look == -2) {

			// MOVE OR COPY THE FILE
			if (action == 0) result2 = MoveFile(path, destination);
			else             result2 = CopyFile(path, destination, true); // TRUE TO NOT OVERWRITE
			if (!result2) return(blank);

			// EXIT LOOP
			break;
		}
	}

	// IF REQUESTED, SET ATTRIBUTES TO MAKE THE FILE VISIBLE
	if (attributes == 1) {
		
		result = FileSet(destination, FILE_ATTRIBUTE_ARCHIVE);
		if (!result) return(blank);
	}

	// RETURN THE DESTINATION PATH TO REPORT SUCCESS
	return(destination);
}

bool FileShortcut(read path, read target, read description)
{
	// takes a path for the .lnk file, the target path the shortcut should point to, and description text for the shortcut
	// creates the shortcut file
	// returns true if successful, false if any error

	// CHECK THE PATH, CHECK THE FOLDERS, AND DELETE ANY SHORTCUT FILE ALREADY PRESENT
	if (!FileCheckPath(path) || !FileFolder(path, 1) || !FileDelete(path)) return(false);

	// GET PSL, A POINTER TO THE I SHELL LINK INTERFACE
	OleInitialize(NULL);
	IShellLink *shelllink;
	HRESULT result;
	result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&shelllink);
	if (!SUCCEEDED(result)) return(false);

	// GET PPF, A POINTER TO THE I PERSIST FILE INTERFACE
	IPersistFile *persistfile;
	result = shelllink->QueryInterface(IID_IPersistFile, (void **)&persistfile);
	if (!SUCCEEDED(result)) { shelllink->Release(); return(false); }

	// SET THE TARGET PATH AND DESCRIPTION TEXT FOR THE SHORTCUT
	shelllink->SetPath((LPCSTR)target);
	shelllink->SetDescription(description);

	// CONVERT THE PATH INTO UNICODE
	WORD pathunicode[MAX_PATH +SAFETY];
	MultiByteToWideChar(CP_ACP, 0, path, -1, (LPWSTR)pathunicode, MAX_PATH);

	// HAVE THE SYSTEM CREATE THE SHORTCUT
	persistfile->Save((LPCOLESTR)pathunicode, true);

	// RELEASE THE POINTER TO I PERSIST FILE AND THEN I SHELL LINK
	persistfile->Release();
	shelllink->Release();
	OleUninitialize();
	return(true);
}

bool FileSame(read path1, read path2)
{
	// takes two paths
	// opens the files and compares them byte by byte
	// returns true if the two files have identical data or are both empty, false if they are different, only one empty, or any error

	// CHECK THAT BOTH FILES ARE PRESENT AND HAVE IDENTICAL POSITIVE SIZE
	int look1, look2;
	look1 = FileLook(path1);
	look2 = FileLook(path2);
	if (look1 < 0 || look2 < 0)   return(false); // LOOKING CAUSED ERROR, NOT FOUND, OR FOLDER HERE, REPORT DIFFERENT
	if (look1 == 0 && look2 == 0) return(true);  // BOTH FILES FOUND AND EMPTY, REPORT SAME
	if (look1 != look2)           return(false); // BOTH FILES FOUND AND SIZES ARE DIFFERENT, REPORT DIFFERENT

	// OPEN THE FILES
	DWORD size1, size2;
	HANDLE file1, file2;
	HANDLE map1, map2;
	LPVOID view1, view2;
	size1 = FileReadOpen(path1, &file1, &map1, &view1);
	if (!size1) return(false); // ERROR
	size2 = FileReadOpen(path2, &file2, &map2, &view2);
	if (!size2) { FileReadClose(file1, map1, view1); return(false); } // ERROR

	// COMPARE SIZES
	if (size1 != size2) {

		FileReadClose(file1, map1, view1);
		FileReadClose(file2, map2, view2);
		return(false);
	}

	// COMPARE DATA
	PBYTE data1, data2;
	data1 = (PBYTE)view1;
	data2 = (PBYTE)view2;
	DWORD i;
	for (i = 0; i < size1; i++) {

		if (data1[i] != data2[i]) {

			FileReadClose(file1, map1, view1);
			FileReadClose(file2, map2, view2);
			return(false);
		}
	}

	// THEY ARE THE SAME
	FileReadClose(file1, map1, view1);
	FileReadClose(file2, map2, view2);
	return(true);
}

bool FileFolder(read path, bool file)
{
	// takes a drive or network file path or folder, drive letter, or share name path with or without a trailing slash, and true if this is a file path
	// checks and makes the chain of folders, building onto existing folders and creating new ones
	// returns true if successful, false if any error

	// COPY THE PATH TO A STRING SO YOU CAN CHANGE IT
	string left, middle, right;
	left = path;

	// REMOVE ANY TRAILING BACKSLASH AND FILE NAME
	if (file) left = before(left, "\\", Reverse, Different);
	left = off(left, "\\", Reverse, Different);

	// A DRIVE
	if (length(left) == 2 && trails(left, ":")) return(true);

	// ALREADY A FOLDER
	if (FileLook(left) == -1) return(true);

	// DRIVE PATH
	if (find(left, ":\\") == 1) {

		// MAKE LEFT LIKE "C:" AND RIGHT LIKE "folder\folder\folder"
		split(left, ":\\", &left, &right);
		left += ":";

	// NETWORK PATH
	} else if (starts(left, "\\\\")) {

		// MAKE LEFT LIKE "\\computer\share" AND RIGHT LIKE "folder\folder\folder"
		left = off(left, "\\");
		split(left, "\\", &left, &middle);
		split(middle, "\\", &middle, &right);
		left = "\\\\" + left + "\\" + middle;
		middle = "";
	}

	// INVALID PATH
	if (isblank(left) || isblank(right)) return(false);

	// LOOP WHILE RIGHT HAS TEXT
	int look, result;
	while (is(right)) {

		// CAT ONTO LEFT A BACKSLASH AND THE FIRST FOLDER FROM RIGHT
		split(right, "\\", &middle, &right);
		left += "\\" + middle;

		// PATH AVAILIABLE
		look = FileLook(left);
		if (look == -2) {

			// MAKE A FOLDER
			result = CreateDirectory(left, NULL);
			if (!result) return(false);

		// PATH IN USE BY FOLDER
		} else if (look == -1) {

		// ERROR OR FILE PRESENT
		} else {

			return(false);
		}
	}

	// REPORT SUCCESS
	return(true);
}

int FileLook(read path)
{
	// takes a drive or network file, folder, drive letter, or share name path that can have or not have a trailing slash
	// determines if the path is already in use by a file or folder, and gets the file size
	// returns -3 error, -2 path availiable, -1 path in use by folder, 0+ path in use by file of that size

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(-3);

	// COPY THE PATH TO A STRING AND REMOVE ANY TRAILING BACKSLASHES
	string s;
	s = path;
	s = off(s, "\\", Reverse, Different);

	// CALL FIND FIRST FILE
	HANDLE findfile;
	WIN32_FIND_DATA info;
	int error;
	findfile = FindFirstFile(s, &info);
	error = GetLastError();

	// THE SEARCH FAILED
	if (findfile == INVALID_HANDLE_VALUE) {

		// CLOSE THE SEARCH
		FindClose(findfile);

		// NOTHING WAS FOUND, REPORT -2 PATH AVAILIABLE
		if (error == ERROR_FILE_NOT_FOUND) {
			
			return(-2);

		// CHECK THE NETWORK PATH BY CREATING A FILE THERE
		} else if (error == ERROR_BAD_NETPATH) {

			// COMPOSE A PATH TO A FILE IN THE FOLDER
			string check;
			check = TextGuid();
			check = make(path, "\\program ", check, ".db");

			// CREATE THE FILE
			HANDLE file;
			file = FileWriteOpen(check, 2); // HIDDEN AND SYSTEM

			// THE FILE WAS CREATED, CLOSE AND DELETE IT AND REPORT -1 PATH IN USE BY FOLDER
			if (file) {

				FileWriteClose(file);
				FileDelete(check);
				return(-1);

			// THE FILE COULD NOT BE CREATED, REPORT -3 ERROR
			} else {

				return(-3);
			}

		// ERROR
		} else {
			
			return(-3);
		}
	}

	// CLOSE THE SEARCH
	FindClose(findfile);

	// GET ATTRIBUTES TO DETERMINE IF PATH IS A FILE OR FOLDER
	DWORD attributes;
	attributes = GetFileAttributes(s);
	if (attributes == -1)                      return(-3);                     // ERROR
	if (attributes & FILE_ATTRIBUTE_DIRECTORY) return(-1);                     // REPORT -1 PATH IN USE BY FOLDER
	else                                       return((int)info.nFileSizeLow); // REPORT 0+ PATH IN USE BY FILE OF THAT SIZE
}

bool FileDelete(read path)
{
	// takes a drive or network file or folder path that has no trailing slash
	// deletes the file or folder, not stopped by any attributes, can't delete folders with contents
	// returns true if the file was deleted successfully or not found to begin with, or false if error

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(false);

	// GET ATTRIBUTES TO DETERMINE IF PATH IS A FILE OR FOLDER
	DWORD attributes, error;
	attributes = GetFileAttributes(path);
	error = GetLastError();
	if (attributes == -1) {

		// ERROR
		if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return(true);  // CAN'T DELETE BECAUSE THE PATH IS AVAILIALBE, RETURN SUCCESS
		else                                                                return(false); // OTHER ERROR ACCESSING THE FILE
	}

	// REMOVE ATTRIBUTES SO THEY WON'T PREVENT DELETION
	FileSet(path, FILE_ATTRIBUTE_NORMAL);

	int result;
	if (attributes & FILE_ATTRIBUTE_DIRECTORY) {

		// CALL REMOVE DIRECTORY
		result = RemoveDirectory(path);
		if (!result) { FileSet(path, attributes); return(false); } // ERROR, RESTORE ORIGINAL ATTRIBUTES AND RETURN

	} else {

		// CALL DELETE FILE
		result = DeleteFile(path);
		if (!result) { FileSet(path, attributes); return(false); } // ERROR, RESTORE ORIGINAL ATTRIBUTES AND RETURN
	}

	// RETURN SUCCESS
	return(true);
}

bool FileSet(read path, DWORD attributes)
{
	// takes a drive or network file or folder path that has no trailing slash, and file attributes
	// sets the file or folder at the path to these attributes
	// returns true if successful, false if any error

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(false);

	// CALL SET FILE ATTRIBUTES
	int result;
	result = SetFileAttributes(path, attributes);
	if (!result) return(false); // ERROR

	// RETURN SUCCESS
	return(true);
}

DWORD FileReadOpen(read path, HANDLE *file, HANDLE *map, LPVOID *view)
{
	// takes a path and pointers to file and map handles and view pointer
	// opens and maps the file at this path
	// returns the size of the file accessible through the view pointer, or 0 if empty file or any error

	// BLANK GIVEN PARAMETERS TO BE ABLE TO RETURN ON ERRORS
	*file = NULL;
	*map  = NULL;
	*view = NULL;

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(0);

	// OPEN THE FILE
	HANDLE f;
	f = CreateFile(
		path,                      // PATH AND FILE NAME
		GENERIC_READ,              // GET READ ONLY ACCESS
		FILE_SHARE_READ,           // ALLOW OTHER CALLS AND PROCESSES TO READ THE FILE WHILE IT IS OPEN
		NULL,
		OPEN_EXISTING,             // OPEN THE FILE HERE, FAILING IF THERE ISN'T ONE
		FILE_FLAG_SEQUENTIAL_SCAN, // TELL WINDOWS THE PROGRAM PLANS TO READ THE FILE FRONT TO END
		NULL);
	if (f == INVALID_HANDLE_VALUE) { Report("filereadopen: error createfile"); return(0); } // ERROR

	// GET THE FILE SIZE
	DWORD s;
	s = GetFileSize(
		f,     // OPEN FILE HANDLE
		NULL); // NULL TO NOT REQUEST THE HIGH ORDER WORD OF THE FILE SIZE
	if (s == -1) { CloseHandleSafely(f); return(0); } // ERROR

	// CHECK THAT THE FILE HAS DATA
	if (s == 0) { CloseHandleSafely(f); return(0); }

	// OPEN THE MAP
	HANDLE m;
	m = CreateFileMapping(
		f,             // HANDLE TO AN OPEN FILE
		NULL,
		PAGE_READONLY, // READ ONLY
		0,             // MAP THE WHOLE FILE
		0,
		NULL);         // DON'T SPECIFY A NAME FOR THE OBJECT
	if (!map) { Report("filereadopen: error createfilemapping"); CloseHandleSafely(f); return(0); } // ERROR

	// OPEN THE VIEW
	LPVOID v;
	v = MapViewOfFile(
		m,             // HANDLE TO AN OPEN MAP
		FILE_MAP_READ, // READ ONLY
		0,             // POINT TO THE START OF THE FILE
		0,
		0);            // MAP THE ENTIRE FILE
	if (!v) { Report("filereadopen: error mapviewoffile"); CloseHandleSafely(m); CloseHandleSafely(f); return(0); } // ERROR

	// STORE VALUES AND RETURN SIZE
	*file = f;
	*map  = m;
	*view = v;
	return(s);
}

bool FileReadClose(HANDLE file, HANDLE map, LPVOID view)
{
	// takes an open file handle, map handle, and view pointer
	// closes the mapping and the file
	// returns true if successful, false if any error

	// CHECK THE HANDLES
	if (!file || !map) return(false);

	// CLOSE THE VIEW
	int result1;
	result1 = UnmapViewOfFile(view);
	if (!result1) { Report("filereadclose: error unmapviewoffile"); return(false); } // ERROR

	// CLOSE THE MAP
	bool result2;
	result2 = CloseHandleSafely(map);
	if (!result2) return(false); // ERROR

	// CLOSE THE FILE
	bool result3;
	result3 = CloseHandleSafely(file);
	if (!result3) return(false); // ERROR

	// RETURN SUCCESS
	return(true);
}

HANDLE FileWriteOpen(read path, int attributes)
{
	// takes a path and 0 normal, 1 hidden must be used to open an already hidden path, or 2 hidden and system
	// creates a new, empty, open file at this path, overwriting and removing the attributes of anything there
	// returns a handle to the new file, or null if any error

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(NULL);

	// PREPARE FLAGS, HIDDEN MAKES NEW AND OPENED NORMAL FILES HIDDEN, BUT FAILS WHEN 0 AND PATH IS AN EXISTING HIDDEN FILE
	DWORD flags;
	if      (attributes == 0) flags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL;
	else if (attributes == 1) flags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_HIDDEN;
	else                      flags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

	// CREATE THE FILE, FAILS IF EXISTING FILE IS READ ONLY OR SYSTEM, OR HIDDEN AND ATTRIBUTE HIDDEN IS FALSE
	HANDLE file;
	file = CreateFile(
		path,          // PATH AND FILE NAME
		GENERIC_WRITE, // GET WRITE ONLY ACCESS
		0,             // ALL OTHER ATTEMPTS TO READ OR WRITE THE FILE WILL FAIL UNTIL IT IS CLOSED
		NULL,
		CREATE_ALWAYS, // CREATE A NEW FILE HERE, OVERWRITING ANYTHING PRESENT
		flags,         // NORMAL OR HIDDEN, AND SEQUENTIAL SCAN
		NULL);
	if (file == INVALID_HANDLE_VALUE) { Report("filewriteopen: error createfile"); return(NULL); }

	// RETURN THE OPEN FILE HANDLE
	return(file);
}

bool FileWriteText(HANDLE file, read r)
{
	// takes an open file handle and text
	// writes the characters into the end of the file
	// returns true if successful, false if any error

	// WRITE THE NUMBER OF CHARACTERS IN THE TEXT MULTIPLIED BY THE SIZE OF EACH CHARACTER
	return(FileWrite(file, (LPVOID)r, length(r) * sizeof(character)));
}

bool FileWrite(HANDLE file, LPVOID memory, DWORD size)
{
	// takes an open file handle, a pointer to a memory block, and the number of bytes there
	// writes the bytes into the end of the file
	// returns true if successful, false if any error

	// CHECK FILE HANDLE AND SIZE
	if (!file || size <= 0) return(false);

	// WRITE DATA TO THE END OF THE FILE, MOVING THE FILE POINTER TO THE NEW END
	BOOL result;
	DWORD written;
	result = WriteFile(
		file,     // OPEN FILE HANDLE
		memory,   // POINTER TO DATA BUFFER
		size,     // NUMBER OF BYTES TO WRITE
		&written, // THE NUMBER OF BYTES WRITTEN IS STORED HERE
		NULL);
	if (!result) { Report("filewrite: error writefile"); return(false); } // ERROR

	// CHECK THAT ALL THE BYTES WERE WRITTEN
	if (size != written) { Report("filewrite: written different from size"); return(false); }

	// RETURN SUCCESS
	return(true);
}

bool FileWriteClose(HANDLE file)
{
	// takes a file handle that was opened for writing
	// closes the file
	// returns true if successful, false if any error

	// CHECK HANDLE
	if (!file) return(false);

	// CLIP THE FILE TO ITS NEWLY WRITTEN LENGTH, NOTE THAT TESTING IN WINDOWS 2000 SHOWS THAT THIS MAY NOT BE NECESSARY
	int result1;
	result1 = SetEndOfFile(file);
	if (!result1) { Report("filewriteclose: error setendofile"); return(false); } // ERROR

	// CLOSE THE HANDLE
	bool result2;
	result2 = CloseHandleSafely(file);
	if (!result2) return(false); // ERROR

	// RETURN SUCCESS
	return(true);
}

bool FileCheckPath(read path)
{
	// takes a path
	// determines if it looks like a valid drive or network file, folder, drive letter, or share name path
	// returns true if so, false if not

	// MAKE SURE THE PATH IS NOT TOO LONG
	if (length(path) > MAX_PATH) return(false);

	// CHECK THE BASIC FORMAT OF THE PATH
	if      (length(path) == 2 && path[1] == ':') return(true);  // PATH IS LIKE "C:"
	else if (find(path, ":\\") == 1)              return(true);  // PATH IS LIKE "C:\path"
	else if (starts(path, "\\\\"))                return(true);  // PATH IS LIKE "\\path"
	else                                          return(false);
}

bool FileList(read folder, read search, HANDLE *findfile, string *path)
{
	// takes a path to a folder with a trailing slash, search text, a pointer to a handle which should be set to null to start, and access to a string
	// finds all the files and folders in the folder
	// returns true if path found, or false and blanks if none, no more, or any error

	// USED BY FIND FUNCTIONS
	WIN32_FIND_DATA info;

	// START
	if (!*findfile) {

		// COMPOSE THE SEARCH PATH TEXT AND START THE FIND
		string s;
		s = make(folder, search);
		*findfile = FindFirstFile(s, &info);
		if (*findfile == INVALID_HANDLE_VALUE) { *path = ""; return(false); }

		// ASSEMBLE THE PATH TO THE FOUND FILE, APPENDING A BACKSLASH IF IT IS A FOLDER, AND RETURN A FOUND PATH
		*path = make(folder, info.cFileName);
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) *path += "\\";
		if (!same(info.cFileName, ".") && !same(info.cFileName, "..")) return(true);
	}

	// LOOP WHILE FILES ARE FOUND
	while (FindNextFile(*findfile, &info)) {

		// ASSEMBLE THE PATH TO THE FOUND FILE, APPENDING A BACKSLASH IF IT IS A FOLDER, AND RETURN A FOUND PATH
		*path = make(folder, info.cFileName);
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) *path += "\\";
		if (!same(info.cFileName, ".") && !same(info.cFileName, "..")) return(true);
	}

	// FIND NEXT FILE RETURNED FALSE BECAUSE THERE ARE NO MORE FILES OR THERE WAS SOME ERROR
	FindClose(*findfile);
	*path = "";
	return(false);
}

void FileRun(read path, read parameters)
{
	// takes a path to a file and parameters text or blank
	// shell executes it
	// returns nothing

	// SHELL EXECUTE THE GIVEN TEXT
	ShellExecute(
		Handle.window,  // HANDLE TO A WINDOW TO GET MESSAGE BOXES FROM THIS OPERATION
		NULL,           // DEFAULT RUN
		path,           // FILE TO RUN
		parameters,     // PARAMETERS TO PASS TO PROGRAM
		"C:\\",         // STARTING DIRECTORY
		SW_SHOWNORMAL); // DEFAULT SHOW
}

string FileBrowse(read display)
{
	// takes text to display in the dialog box
	// displays the browse for folder dialog box
	// returns the path the user chose, or blank if cancel or error

	// SETUP A BROWSEINFO STRUCTURE FOR BROWSE FOR FOLDER
	character name[MAX_PATH +SAFETY];
	BROWSEINFO info;
	info.hwndOwner      = Handle.window;        // HANDLE TO PARENT WINDOW FOR THE BROWSE DIALOG
	info.pidlRoot       = NULL;                 // BROWSE FROM THE DESKTOP
	info.pszDisplayName = name;                 // WRITE THE NAME OF THE CHOSE FOLDER HERE
	info.lpszTitle      = display;              // TEXT TO DISPLAY IN THE BROWSE DIALOG
	info.ulFlags        = BIF_RETURNONLYFSDIRS; // ONLY ALLOW FILE SYSTEM FOLDERS
	info.lpfn           = NULL;                 // NO CALLBACK FUNCTION
	info.lParam         = 0;                    // OPTIONAL TO SPECIFY A MEANINGFULL LPARAM
	info.iImage         = 0;                    // WILL BE FILLED WITH THE ICON INDEX OF THE SELECTED ITEM

	// DISPLAY THE BROWSE FOR FOLDER SYSTEM DIALOG BOX
	OleInitialize(NULL);
	LPITEMIDLIST result;
	result = SHBrowseForFolder(&info);
	OleUninitialize();
	if (!result) return(blank); // THE USER CANCELLED FROM THE BROWSE FOR FOLDER DIALOG

	// GET THE PATH OF THE FOLDER THE USER CHOSE AND COPY IT INTO PATH
	character bay[MAX_PATH +SAFETY];
	SHGetPathFromIDList(result, bay);
	return(make(bay));
}

string FileChoose()
{
	// takes nothing
	// shows the file open dialog box to let the user choose a file
	// returns its path, or blank if not chosen or any error

	// SHOW THE FILE OPEN DIALOG BOX AND READ THE PATH IF THE USER CLICKS OK
	string s;
	OPENFILENAME info;
	character name[MAX_PATH];
	name[0] = '\0';
	info.lStructSize = sizeof(info);
	info.hwndOwner = Handle.window;
	info.lpstrFilter = NULL;
	info.lpstrCustomFilter = NULL;
	info.nFilterIndex = 0;
	info.lpstrFile = name;
	info.nMaxFile = MAX_PATH;
	info.lpstrFileTitle = NULL;
	info.lpstrInitialDir = NULL;
	info.lpstrTitle = NULL;
	info.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	info.lpstrDefExt = NULL;
	info.lCustData = 0;
	if (GetOpenFileName(&info)) s = info.lpstrFile;

	// RETURN THE PATH OR BLANK IF THE USER CLICKED CANCEL
	return(s);
}

string ListFolders(read folder)
{
	// takes a folder path with a trailing backslash
	// finds all the subfolders in it
	// returns their paths in a string separated by \n characters

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE h;
	h = NULL;
	string path, folders;
	while (FileList(folder, "*.*", &h, &path)) {

		// FROM IS A FOLDER
		if (trails(path, "\\")) folders += path + "\n";
	}

	// RETURN THE LIST OF FOLDER PATHS SEPARATED BY NEW LINE CHARACTERS BETWEEN THEM
	return(trim(folders, "\n"));
}
