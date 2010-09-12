
// Include platform
#include <windows.h>
#include <windef.h>
#include <atlstr.h>

// Include program
#include "resource.h"
#include "program.h"
#include "function.h"

// Access to global objects
extern handletop Handle;

bool CloseHandleSafely(HANDLE h)
{
	// takes an open handle that can be closed by close handle
	// closes it
	// returns true if successful, false otherwise

	// CHECK THE HANDLE
	if (!h || h == INVALID_HANDLE_VALUE) { Report("closehandlesafely: bad parameters"); return(false); } // ERROR

	// CLOSE THE HANDLE
	int result;
	result = CloseHandle(h);
	if (!result) { Report("closehandlesafely: error closehandle"); return(false); } // ERROR

	// RETURN SUCCESS
	return(true);
}

void Report(read r)
{
	// takes text
	// displays a message box if the program is test, otherwise does nothing
	// returns nothing

	// IF THE PROGRAM IS COMPILED TO REPORT ERRORS DISPLAY THE TEXT IN A MESSAGE BOX
	if (PROGRAM_TEST) MessageBox(Handle.window, r, "Test", MB_OK);
}
