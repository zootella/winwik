
// Include
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
		PROGRAM_TITLE,                          // Text to show in the title bar, or null for no text
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

void DialogUpdate()
{
	// call to update the dialog box and process messages to paint it

	// FORCE THE DIALOG BOX TO REDRAW RIGHT NOW
	RedrawWindow(Handle.dialog, NULL, NULL, RDW_UPDATENOW);

	// CALL PEEK MESSAGE TO LET THE WINDOW REDRAW, DOING THIS CAUSES USER.EXE IN WINDOWS 95/98/ME TO CRASH
	LPMSG message;
	message = NULL;
	while (PeekMessage(message, NULL, 0, 0, PM_REMOVE)) {

		TranslateMessage(message);
		DispatchMessage(message);
	}
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uiMessage) {

		case WM_COMMAND:

			switch (LOWORD(wParam)) {

				// THE USER CLICKED ON A MENU ITEM
				case IDM_TEST: Test(); return(0);
			}

			break;

		// CLOSE THE APPLICATION
		case WM_DESTROY:

			PostQuitMessage(0);
			return(0);
		}

	return DefWindowProc(hWnd, uiMessage, wParam, lParam);
}
