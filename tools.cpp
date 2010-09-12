
// INCLUDE STATEMENTS
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// Global objects
extern handletop Handle;

void List()
{
	// takes nothing
	// performs a list operation
	// returns nothing

	// GET THE PATH TO LIST FROM THE USER
	string path;
	path = FileBrowse("Choose the folder to list.");
	if (isblank(path)) return;

	// LIST ALL THE FILES AND FOLDERS
	string list;
	ListFolder(on(path, "\\", Reverse, Different), &list);

	// COPY THE TEXT LIST TO THE CLIPBOARD AND REPORT THAT THE TASK IS DONE
	ClipboardCopy(list);
	MessageBox(Handle.window, "List copied.", PROGRAM_NAME, MB_OK);
}

void ListFolder(read folder, string *list)
{
	// takes a path to a folder that ends with a trailing slash and access to a string
	// lists the paths of all the files and folders in the folder, cating folders and 0d0a pairs onto the list string
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string path;
	while (FileList(folder, "*.*", &find, &path)) {

		// APPEND THE FOUND PATH TO THE LIST
		*list += path + "\x0D\x0A";

		// IF PATH IS A FOLDER, MOVE INTO IT WITH THIS FUNCTION
		if (trails(path, "\\")) ListFolder(path, list);
	}
}

bool FileMoveSimple(read source, read destination, bool folder, bool copy)
{
	// takes source and destination paths, whether to check and prepare the folder, and whether to move or copy the file
	// performs the desired operations
	// returns true if successful, or false if any error

	// VARIABLES TO HOLD RESULTS
	bool result;
	int result2;

	// FOLDER
	if (folder) {

        result = FileFolder(destination, 1); // PATH INCLUDES FILE NAME
		if (!result) return(false);
	}

	// COPY
	if (copy) {

		result2 = CopyFile(source, destination, true); // DON'T OVERWRITE
		if (!result2) return(false);

	// MOVE
	} else {

		result2 = MoveFile(source, destination);
		if (!result2) return(false);
	}

	// REPORT SUCCESS
	return(true);
}
