
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

void ClipboardCopy(read r)
{
	// takes text
	// copies it onto the clipboard
	// returns nothing

	// MAKE A MEMORY BLOCK LARGE ENOUGH TO HOLD THE TEXT AND ITS TERMINATOR AND GET A HANDLE TO IT
	HANDLE memoryhandle = GlobalAlloc(GHND, length(r) + 1);

	// LOCK THE HANDLE
	LPSTR memorypointer = (LPSTR)GlobalLock(memoryhandle);

	// COPY THE BYTES OF THE TEXT INTO THE MEMORY BLOCK, THIS DOES INCLUDE AND DOES COPY THE NULL TERMINATOR AT THE END OF R
	lstrcpy(memorypointer, r);

	// UNLOCK THE HANDLE
	GlobalUnlock(memoryhandle);

	// COPY THE DATA ONTO THE CLIPBOARD, THE MEMORY IS NO LONGER THE PROGRAM'S TO FREE
	OpenClipboard(Handle.window);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, memoryhandle);
	CloseClipboard();
}
