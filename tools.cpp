
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

void Slice()
{
	// takes nothing
	// slices a tree of folders into an array of discs
	// returns nothing

	// ASK THE USER WHAT FOLDER TO SLICE INTO DISCS
	string folder;
	folder = FileBrowse("Choose the folder to slice into discs.");
	if (isblank(folder)) return;
	folder = on(folder, "\\", Reverse, Different);

	// CONFIRM THE DONE FOLDER DOES NOT ALREADY EXIST
	if (FileLook(clip(folder, 0, length(folder) - 1) + "(done)\\") != -2) { MessageBox(Handle.window, "Done folder already exists.", PROGRAM_NAME, MB_OK); return; }

	//call the recursive function
	int number, size;
	number = 1;
	size = 0;
	SliceFolder(folder, folder, &number, &size);

	// REPORT THAT THE TASK IS DONE
	MessageBox(Handle.window, "File tree sliced.", PROGRAM_NAME, MB_OK);
}

void SliceFolder(read base, read folder, int *number, int *size)
{
	// takes the folder being sliced, that folder or one found within it, and access to the current disc number and disc size
	// slices folders and files
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string from, disc, to;
	int s;
	while (FileList(folder, "*.*", &find, &from)) {

		// FROM IS A FOLDER
		if (trails(from, "\\")) {

			// MOVE INTO THE FOLDER WITH THIS FUNCTION
			SliceFolder(base, from, number, size);

		// FROM IS A FILE
		} else {

			// GET THE FILE SIZE
			s = FileLook(from);
			if (s < 0) MessageBox(Handle.window, "Unable to size '" + from + "'", PROGRAM_NAME, MB_OK);

			// BIGGER THAN A DISC
			if (s > DISCSIZE) {

				// COMPOSE DISC FOLDER
				disc = "(oversize)";

			} else {

				// TOO BIG FOR THIS DISC, PLACE IN THE NEXT ONE
				if (*size + s > DISCSIZE) {

					(*number)++;
					*size = s;

				// SMALL ENOUGH FOR THIS DISC
				} else {

					*size += s;
				}

				// COMPOSE DISC FOLDER
				disc = numerals(*number);
				while (length(disc) < 3) disc = "0" + disc;
			}

			// COMPOSE DESTINATION PATH
			to = clip(base, 0, length(base) - 1) + "(done)\\" + disc + "\\" + clip(from, length(base), -1);

			// MOVE THE FILE, REPORTING ANY ERROR
			if (!FileMoveSimple(from, to, 1, 0)) MessageBox(Handle.window, "Unable to move '" + from + "' to '" + to + "'", PROGRAM_NAME, MB_OK);
		}
	}
}
