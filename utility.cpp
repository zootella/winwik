
// Include platform
#include <windows.h>
#include <windef.h>
#include <atlstr.h>

// Include program
#include "resource.h"
#include "define.h"
#include "function.h"

// Access to global objects
extern handletop Handle;

// Print an error message out on the output log
void error(read r1, read r2, read r3, read r4, read r5, read r6, read r7, read r8, read r9) {

	if (PROGRAM_TEST) log(make("error ", numerals(GetLastError()), " ", make(r1, r2, r3, r4, r5, r6, r7, r8, r9)));
}

// Print a message out on the output log
void log(read r1, read r2, read r3, read r4, read r5, read r6, read r7, read r8, read r9) {

	if (!PROGRAM_TEST) return;
	CString s = make(make(r1, r2, r3, r4, r5, r6, r7, r8, r9), "\r\n");
	OutputDebugString(s);
}

// Show a message to the user
void report(read r) {

	if (PROGRAM_TEST) MessageBox(Handle.window, r, PROGRAM_NAME, MB_OK);
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
		file,     // OPEN FILE HANDLE
		(LPVOID)r,   // POINTER TO DATA BUFFER
		size,     // NUMBER OF BYTES TO WRITE
		&written, // THE NUMBER OF BYTES WRITTEN IS STORED HERE
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
