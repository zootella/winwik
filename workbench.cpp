
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




void ClipboardChanged() {


	string s;
	if (ClipboardPaste(&s))
		FileSave("C:\\Documents\\clipboard.txt", s);



}


