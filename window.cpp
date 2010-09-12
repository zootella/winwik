
// INCLUDE STATEMENTS
#include <windows.h>
#include <windef.h>
#include <atlstr.h>
#include <shlobj.h>
#include "resource.h"
#include "class.h"
#include "program.h"

// GLOBAL VARIABLES
handletop Handle; // GLOBAL HANDLE STRUCTURE
statetop  State;  // STATE VARIABLES

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASSEX wndclass;

	// NULL DATA LISTS
	State.rename.r = NULL;

	// SAVE HANDLES
	Handle.instance = hInstance;    // SAVE THE INSTANCE IN A GLOBAL VARIABLE
	Handle.heap = GetProcessHeap(); // SAVE THE HANDLE TO THE PROCESS HEAP IN A GLOBAL VARIABLE

	// REGISTER THE CLASS FOR THE MAIN WINDOW
	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = 0;
	wndclass.lpfnWndProc   = MainWinProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = Handle.instance;
	wndclass.hIcon         = (HICON)LoadImage(Handle.instance, "APPICON", IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	wndclass.hIconSm       = (HICON)LoadImage(Handle.instance, "APPICON", IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wndclass.lpszMenuName  = "MAINMENU";
	wndclass.lpszClassName = PROGRAMNAME;
	RegisterClassEx(&wndclass);

	// CREATE THE MAIN WINDOW
	Handle.window = CreateWindow(
		PROGRAMNAME,
		PROGRAMTITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		100,
		NULL,
		NULL,
		Handle.instance,
		(LPVOID)NULL);

	// ADD THE TEST BUTTON
	if (PROGRAMTEST) AppendMenu(GetMenu(Handle.window), MF_STRING, IDM_TEST, "Test");

	// MAKE BRUSHES AND FONT
	Handle.blue        = BrushColor(RGB(  0, 102, 204));
	Handle.lightblue   = BrushColor(RGB( 51, 153, 255));
	Handle.yellow      = BrushColor(RGB(255, 204,   0));
	Handle.lightyellow = BrushColor(RGB(255, 255, 102));
	Handle.green       = BrushColor(RGB(102, 204,  51));
	Handle.lightgreen  = BrushColor(RGB(153, 255, 102));
	Handle.red         = BrushColor(RGB(255, 102,  51));
	Handle.lightred    = BrushColor(RGB(255, 153, 102));
	Handle.arial = CreateFont("Arial", 90);

	// SHOW THE MAIN WINDOW AND PAINT IT NOW
	ShowWindow(Handle.window, SW_SHOWNORMAL);
	UpdateWindow(Handle.window);

	// ENTER THE MESSAGE LOOP
	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
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

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
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
