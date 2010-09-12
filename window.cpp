
// Include headers
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// Global objects
handletop Handle;

// Start the program
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, PSTR command, int show) {

	// Save the given instance handle
	Handle.instance = instance;

	// Register the class for the main window
	string name = make(PROGRAM_NAME, "ClassName"); // Compose a unique window class name
	WNDCLASSEX info;
	ZeroMemory(&info, sizeof(info));
	info.cbSize        = sizeof(info);
	info.style         = 0;
	info.lpfnWndProc   = WindowProcedure;
	info.cbClsExtra    = 0;
	info.cbWndExtra    = 0;
	info.hInstance     = Handle.instance;
	info.hIcon         = (HICON)LoadImage(Handle.instance, "APPICON", IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	info.hIconSm       = (HICON)LoadImage(Handle.instance, "APPICON", IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	info.hCursor       = LoadCursor(NULL, IDC_ARROW);
	info.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	info.lpszMenuName  = "MAINMENU";
	info.lpszClassName = name;
	RegisterClassEx(&info);

	// Create the main window
	Handle.window = CreateWindow(
		name,                                   // System or registered window class name or class
		PROGRAM_NAME,                           // Text to show in the title bar, or null for no text
		WS_OVERLAPPEDWINDOW,                    // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, // Window position and size
		NULL,                                   // Handle to parent window
		NULL,                                   // Menu handle or child window identification number
		Handle.instance,                        // Program instance handle
		NULL);                                  // No parameter

	// Show the main window and paint it now
	ShowWindow(Handle.window, SW_SHOWNORMAL);
	UpdateWindow(Handle.window);

	// Enter the message loop
	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {

		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return (int)message.wParam;
}

// Process a message from the system
LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

	// The message is a command
	switch (message) {
	case WM_COMMAND:

		switch (LOWORD(wparam)) {

			// The user clicked on a menu item
			case IDM_TEST: Test(); return 0;
		}

	// Close the program
	break;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(window, message, wparam, lparam);
}
