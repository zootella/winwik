
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

string TextDialog(HWND dialog, int control)
{
	// takes a dialog handle and a dialog control identifier number
	// gets the text of the control
	// returns a string, blank if no text or any error

	// GET A HANDLE TO THE WINDOW OF THE CONTROL AND RETURN THE TEXT OF THAT WINDOW
	return(TextWindow(GetDlgItem(dialog, control)));
}

string TextWindow(HWND window)
{
	// takes a window handle
	// gets the text of the window
	// returns a string, blank if no text or any error

	// IF THE WINDOW HANDLE IS NULL RETURN BLANK
	if (!window) return(blank);

	// GET THE TEXT LENGTH
	int n;
	n = (int)SendMessage(window, WM_GETTEXTLENGTH, 0, 0);

	// OPEN A STRING
	string s;
	write buffer = s.GetBuffer(n);
	buffer[n] = '\0';

	// WRITE THE WINDOW TEXT INTO THE BUFFER
	GetWindowText(
		window, // HANDLE TO WINDOW
		buffer, // TEXT BUFFER
		n + 1); // SIZE OF BUFFER

	// CLOSE THE STRING
	s.ReleaseBuffer();

	// RETURN THE STRING
	return(s);
}

string TextGuid()
{
	// takes nothing
	// generates a guid
	// returns a string with the 32 lowercase hexidecimal characters of the guid, or blank if any error

	// GET A NEW UNIQUE GUID FROM THE SYSTEM
	GUID guid;
	HRESULT result;
	result = CoCreateGuid(&guid);
	if (result != S_OK) { Report("error co create guid"); return(blank); }

	// CONVERT THE GUID INTO AN OLE WIDE CHARACTER STRING
	WCHAR bay[MAX_PATH +SAFETY];
	int characters;
	characters = StringFromGUID2(
		guid,          // GUID TO CONVERT
		(LPOLESTR)bay, // WRITE TEXT HERE
		MAX_PATH);     // SIZE OF BAY
	if (!characters) { Report("error string from guid 2"); return(blank); }

	// CONVERT THE OLE WIDE CHARACTER STRING INTO A TEXT STRING
	COLE2T text(bay);
	string s;
	s = text;

	// CLIP OUT THE NUMBER PARTS OF THE GUID STRING AND LOWERCASE IT
	s = lower(clip(s, 1, 8) + clip(s, 10, 4) + clip(s, 15, 4) + clip(s, 20, 4) + clip(s, 25, 12));

	// MAKE SURE THE GUID STRING IS 32 DIGITS
	if (length(s) != 32) { Report("text guid did not result in a 32-character guid"); return(blank); }

	// RETURN THE STRING
	return(s);
}
