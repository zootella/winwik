
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

// Run a snippet of test code
void Test() {



	FileSave("C:\\Documents\\test\\file.txt", "hello inside the file");
}

