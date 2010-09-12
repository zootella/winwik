
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

// Show a message to the user
void Report(read r) {

	if (PROGRAM_TEST) MessageBox(Handle.window, r, PROGRAM_NAME, MB_OK);
}
