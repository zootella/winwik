
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

// Create global objects
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
	info.lpszMenuName  = NULL;
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

	// Join the clipboard viewer chain
	ClipboardJoin();

	// Enter the message loop
	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// Leave the clipboard viewer chain
	ClipboardLeave();
	return message.wParam;
}

// Process a message from the system
LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

	// Another window is exiting the clipboard viewer chain
	switch (message) {
	case WM_CHANGECBCHAIN:

		// Unlink the next program or pass the message along
		if (Handle.viewer == (HWND)wparam) Handle.viewer = (HWND)lparam;
		else if (Handle.viewer) SendMessage(Handle.viewer, message, wparam, lparam);
		return 0;

	// The clipboard contents have changed
	break;
	case WM_DRAWCLIPBOARD:

		// Send the messate to the next window in the clipboard viewer chain, then process it here
		if (Handle.viewer) SendMessage(Handle.viewer, message, wparam, lparam);
		ClipboardChanged();
		return 0;

	// Close the program
	break;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	// Give the message to the system
	return DefWindowProc(window, message, wparam, lparam);
}
