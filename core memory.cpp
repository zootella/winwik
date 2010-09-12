
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

bool KillTimerSafely(HWND window, int timer)
{
	// takes a window handle and a timer identifier number of a timer in that window
	// kills it
	// returns true if successful, false otherwise

	// KILL THE TIMER
	int result;
	result = KillTimer(window, timer);
	if (!result) { Report("killtimersafely: error killtimer"); return(false); } // ERROR

	// RETURN SUCCESS
	return(true);
}

bool Message(read title, read message, UINT type)
{
	// takes text and options for a message box
	// displays the message box
	// returns true or false for ok cancel and yes no

	// SHOW THE MESSAGE BOX WITH THE MOUSE AWAY
	int choice;
	choice = MessageBox(Handle.window, message, title, type);

	// SORT AND RETURN THE CHOICE
	if (choice == IDOK || choice == IDYES) return(true);
	else return(false);
}

void Report(read r)
{
	// takes text
	// displays a message box if the program is test, otherwise does nothing
	// returns nothing

	// IF THE PROGRAM IS COMPILED TO REPORT ERRORS DISPLAY THE TEXT IN A MESSAGE BOX
	if (PROGRAM_TEST) MessageBox(Handle.window, r, "Test", MB_OK);
}

bool CriticalBox(read r)
{
	// takes text
	// displays a message box to tell the user that a necessary system call had an error
	// exits the program or returns true to try again

	// DISPLAY THE TEXT IN A MESSAGE BOX AND LET THE USER CHOOSE TO RETRY OR EXIT THE PROGRAM
	int choice;
	choice = MessageBox(Handle.window, r, "Error", MB_RETRYCANCEL | MB_ICONSTOP);

	// HALT AND EXIT THE RUNNING INSTANCE RIGHT HERE
	if (choice == IDCANCEL) ExitProcess(0);

	// RETURN TRUE TO LOOP AND TRY AGAIN
	return(true);
}
