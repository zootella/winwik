
// Include statements
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// Global variables
extern handletop Handle; // Global handle structure
extern statetop  State;  // State variables

// Delete and copy files and folders to quickly make the folder at destination like the folder at source
void BackupUpdate(read source, read destination, bool compare) {

	// Remove the trailing slashes, no other paths in this file have them
	string s = off(source,      "\\", Reverse, Different);
	string d = off(destination, "\\", Reverse, Different);

	// Perform the update operation
	UpdateClear(s, d);         // Clear the destination of extra and different files and folders
	UpdateFill(s, d, compare); // Copy missing files and folders in the destination from the source
}

// Delete the folders and files in destination that don't match the source
void UpdateClear(read source, read destination) {

	// Loop through the contents of the destination folder
	BackupDisplayTask(make("Clearing •", destination, "•"));
	HANDLE h = NULL;
	WIN32_FIND_DATA info;
	string s, d;
	while (UpdateList(destination, &h, &info)) {
		s = make(source,      "\\", info.cFileName);          // The path that matches on the source side
		d = make(destination, "\\", info.cFileName);          // The path we found on the destination side

		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Found a subfolder on the destination side
			if (UpdateIsFolder(s))                            // If there is also a folder on the source side
				UpdateClear(s, d);                            // Move into the pair of folders
			else                                              // Otherwise there is a file or nothing on the source side
				UpdateDeleteFolder(d);                        // Delete the folder on the destination side
		else                                                  // Found a file on the destination side
			if (!UpdateSame(s, &info))                        // If there isn't a file on the source side that matches
				UpdateDeleteFile(d);                          // Delete the file on the destination side
	}
}

// Copy source to destination, not overwriting anything
void UpdateFill(read source, read destination, bool compare) {

	// Loop through the contents of the source folder
	BackupDisplayTask(make("Updating •", source, "•"));
	HANDLE h = NULL;
	WIN32_FIND_DATA info;
	string s, d;
	while (UpdateList(source, &h, &info)) {
		s = make(source,      "\\", info.cFileName);          // The path we found on the source side
		d = make(destination, "\\", info.cFileName);          // The path that matches on the destination side

		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Found a source subfolder
			if (UpdateIsFolder(d))                            // There is also a folder on the destination side
				UpdateFill(s, d, compare);                    // Move into the pair of folders
			else                                              // There isn't a folder on the destination side, so it must be available
				UpdateCopyFolder(s, d, compare);              // Copy the folder across
		else                                                  // Found a source file
			if (!UpdateIsFile(d))                             // If there isn't a file on the destination side, or we can't tell
				UpdateCopyFile(s, d, compare);                // Copy the file across, and optionally compare it
	}
}

// Loop to list all the files and folders in the path folder, set h to null to start
bool UpdateList(read path, HANDLE *h, WIN32_FIND_DATA *info) {

	// Start the search
	if (!*h) {
		*h = FindFirstFile(make(path, "\\*.*"), info);
		if (*h == INVALID_HANDLE_VALUE) return false;

		// Skip over "." and ".." at the start
		if (!same(info->cFileName, ".") && !same(info->cFileName, "..")) return true;
	}

	// Get the next file or folder in the list
	while (FindNextFile(*h, info)) {

		// Skip over "." and ".." at the start
		if (!same(info->cFileName, ".") && !same(info->cFileName, "..")) return true;
	}

	// We're done listing the files
	FindClose(*h);
	return false;
}

// True if the file at path has the same size and last modified date described by info
bool UpdateSame(read path, WIN32_FIND_DATA *info) {

	// Get information about the path
	WIN32_FIND_DATA info2;
	HANDLE h = FindFirstFile(path, &info2);
	if (h == INVALID_HANDLE_VALUE) return false; // File not found or other error, path is not the same
	FindClose(h);

	// Only return true if the sizes and last modified dates are the same
	return
		info->nFileSizeLow  == info2.nFileSizeLow  &&
		info->nFileSizeHigh == info2.nFileSizeHigh &&
		info->ftLastWriteTime.dwLowDateTime  == info2.ftLastWriteTime.dwLowDateTime &&
		info->ftLastWriteTime.dwHighDateTime == info2.ftLastWriteTime.dwHighDateTime;
}

// True if path is to a folder on the disk
bool UpdateIsFolder(read path) {

	// Only return true if we can get the file attributes and they include the directory flag
	DWORD d = GetFileAttributes(path);
	return d != INVALID_FILE_ATTRIBUTES && (d & FILE_ATTRIBUTE_DIRECTORY);
}

// True if path is to a file on the disk
bool UpdateIsFile(read path) {

	// Only return true if we can get the file attributes and they don't include the directory flag
	DWORD d = GetFileAttributes(path);
	return d != INVALID_FILE_ATTRIBUTES && !(d & FILE_ATTRIBUTE_DIRECTORY);
}

// Delete the contents of the folder at path, and then the folder itself
void UpdateDeleteFolder(read path) {

	// Delete the contents of the folder
	HANDLE h = NULL;
	WIN32_FIND_DATA info;
	while (UpdateList(path, &h, &info)) {

		// Delete the subfolder or file
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			UpdateDeleteFolder(make(path, "\\", info.cFileName));
		else
			UpdateDeleteFile(make(path, "\\", info.cFileName));
	}

	// Delete the folder
	UpdateDeleteFile(path);
}

// Copy the folder at source to the destination path
void UpdateCopyFolder(read source, read destination, bool compare) {

	// Make the destination folder
	UpdateMakeFolder(destination);

	// Copy the contents across
	HANDLE h = NULL;
	WIN32_FIND_DATA info;
	while (UpdateList(source, &h, &info)) {

		// Copy the subfolder or file
		if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			UpdateCopyFolder(make(source, "\\", info.cFileName), make(destination, "\\", info.cFileName), compare);
		else
			UpdateCopyFile(make(source, "\\", info.cFileName), make(destination, "\\", info.cFileName), compare);
	}
}

// Delete the file or empty folder at path
void UpdateDeleteFile(read path) {

	// Delete the path
	string message = make("Deleting •", path, "•");
	BackupDisplayTask(message);
	if (!FileDelete(path)) BackupDisplayError(message);
}

// Make a new folder at path
void UpdateMakeFolder(read path) {

	// Make the folder
	string message = make("Foldering •", path, "•");
	BackupDisplayTask(message);
	if (CreateDirectory(path, NULL)) { // NULL to use default security attributes
		State.backup.folder++;
		BackupDisplayStatus();
	} else {
		State.backup.foldererror++;
		BackupDisplayStatus();
		BackupDisplayError(message);
	}
}

// Copy the file at source to the destination path
void UpdateCopyFile(read source, read destination, bool compare) {

	// Copy the file
	string message = make("Copying •", source, "• to •", destination, "•");
	BackupDisplayTask(message);
	if (CopyFile(source, destination, true)) { // true to not overwrite
		State.backup.file++;
		BackupDisplayStatus();
	} else {
		State.backup.fileerror++;
		BackupDisplayStatus();
		BackupDisplayError(message);
	}

	// Compare the files
	if (compare) {
		message = make("Comparing •", source, "• to •", destination, "•");
		BackupDisplayTask(message);
		if (FileSame(source, destination)) {
			State.backup.compare++;
			BackupDisplayStatus();
		} else {
			State.backup.compareerror++;
			BackupDisplayStatus();
			BackupDisplayError(message);
		}
	}
}
