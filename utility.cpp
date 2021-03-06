
// Include platform
#include <windows.h>
#include <windef.h>
#include <atlstr.h>

// Include language
#include <vector>

// Include program
#include "resource.h"
#include "define.h"
#include "function.h"

// Access to global objects
extern handletop Handle;

// Print an error message out on the output log
void error(read r) {

	if (PROGRAM_TEST) MessageBox(
		Handle.window,
		make("error ", numerals(GetLastError()), " ", r),
		PROGRAM_NAME,
		MB_OK);
}

// Reads the text contents of the file at path
string FileOpen(read path) {

	// Open an existing file at path for reading
	HANDLE file = CreateFile(
		path,                      // Path and file name
		GENERIC_READ,              // Only need read access
		0,                         // Don't share access with other processes while we've got it open
		NULL,                      // No security attributes
		OPEN_EXISTING,             // Don't create a file
		FILE_FLAG_SEQUENTIAL_SCAN, // We're going to read in a single pass
		NULL);                     // No template
	if (!file || file == INVALID_HANDLE_VALUE) { error("createfile"); return ""; }

	// Find out how big the file is in bytes
	DWORD size = GetFileSize(file, NULL);

	// Open a string
	string s;
	LPTSTR buffer = s.GetBuffer(size);

	// Copy the file contents into the string
	DWORD did = 0;
	int result = ReadFile(
		file,   // File to read from
		buffer, // Destination buffer
		size,   // Number of bytes to read
		&did,   // Number of bytes read
		NULL);  // No overlapping
	if (!result || size != did) { error("readfile"); CloseHandle(file); s.ReleaseBuffer(0); return ""; }

	// Close the file, close the string, and return it
	CloseHandle(file);
	s.ReleaseBuffer(size); // Must supply size because data isn't null terminated
	return s;
}

// Saves the given text to a file at path, overwriting a file already there
bool FileSave(read path, read r) {

	// Create and open a new empty file at path for writing
	HANDLE file = CreateFile(
		path,                       // Path and file name
		GENERIC_WRITE,              // Only need write access
		0,                          // Don't share access with other processes while we've got it open
		NULL,                       // No security attributes
		CREATE_ALWAYS,              // Overwrite a file already at path
		FILE_FLAG_SEQUENTIAL_SCAN | // We're going to write in a single pass
		FILE_ATTRIBUTE_NORMAL,      // Normal file attributes
		NULL);                      // No template
	if (!file || file == INVALID_HANDLE_VALUE) { error("createfile"); return false; }

	// Write the text into the file
	DWORD size = length(r);
	DWORD written = 0;
	int result = WriteFile(
		file,      // Open file handle
		(LPVOID)r, // Pointer to data buffer
		size,      // Number of bytes to write
		&written,  // Put number of bytes written here
		NULL);
	if (!result)         { error("writefile");        return false; }
	if (size != written) { error("size not written"); return false; }

	// Close the file
	if (!CloseHandle(file)) { error("closehandle"); return false; }
	return true;
}

// Insert this program into the clipboard viewer chain
void ClipboardJoin() {

	// Add our window to the clipboard viewer chain and get the handle to the window next in the chain
	Handle.viewer = SetClipboardViewer(Handle.window);
}

// Remove this program from the clipboard viewer chain
void ClipboardLeave() {

	// Remove our window from the clipboard viewer chain
	ChangeClipboardChain(Handle.window, Handle.viewer);
}

// If the clipboard has text, sets s with it and returns true
bool ClipboardPaste(string *s) {
	
	// Only do something if the clipboard has text
	if (!IsClipboardFormatAvailable(CF_TEXT)) return false;

	// Open the clipboard
	int result = OpenClipboard(Handle.window); // Lock the clipboard to this program
	if (!result) return false;
	HANDLE h = GetClipboardData(CF_TEXT); // Open the handle to the external memory block containing the text
	if (!h) { CloseClipboard(); return false; }
	LPTSTR c = (LPTSTR)GlobalLock(h); // Lock the handle and point a LPTSTR at it, the clipbard text ends with a terminator making this safe

	// Get the text on the clipboard
	*s = c;

	// Close the clipboard
	GlobalUnlock(h);
	CloseClipboard();
	return true;
}
