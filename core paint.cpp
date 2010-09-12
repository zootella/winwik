
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

void deviceitem::OpenUse(HDC newdevice)
{
	// takes a handle to a device context
	// loads it into this object
	// returns nothing

	// RECORD HOW THE DEVICE WAS OPENED
	if (open != DeviceNone) return;
	open = DeviceUse;

	// USE THE DEVICE CONTEXT
	device = newdevice;
}

void deviceitem::OpenCreate()
{
	// takes nothing
	// creates a default display device context for the screen
	// returns nothing

	// RECORD HOW THE DEVICE WAS OPENED
	if (open != DeviceNone) return;
	open = DeviceCreate;

	// CREATE THE DEVICE CONTEXT
	device = CreateDC("DISPLAY", NULL, NULL, NULL);
	if (!device) Report("error createdc");
}

void deviceitem::OpenGet(HWND newwindow)
{
	// takes a window
	// gets its device context
	// returns nothing

	// RECORD HOW THE DEVICE WAS OPENED
	if (open != DeviceNone) return;
	open = DeviceGet;

	// GET THE DEVICE CONTEXT
	window = newwindow;
	device = GetDC(newwindow);
	if (!device) Report("error getdc");
}

void deviceitem::OpenPaint(HWND newwindow)
{
	// takes a window
	// tells the system the program will begin painting
	// returns nothing

	// RECORD HOW THE DEVICE WAS OPENED
	if (open != DeviceNone) return;
	open = DevicePaint;

	// PAINT THE DEVICE CONTEXT
	window = newwindow;
	device = BeginPaint(window, &paint);
	if (!device) Report("error beginpaint");
}

deviceitem::~deviceitem()
{
	// takes nothing
	// restores the contents of the device context and ends or deletes it
	// returns nothing

	// PUT EVERYTHING BACK INTO THE DEVICE CONTEXT
	if (font) SelectObject(device, font);
	if (replacebackground) SetBkMode(device, background);
	if (replacefontcolor) SetTextColor(device, fontcolor);
	if (replacebackgroundcolor) SetBkColor(device, backgroundcolor);

	// CLOSE DEVICE CONTEXT
	if      (open == DeviceCreate) { if (!DeleteDC(device))          Report("error deletedc"); }
	else if (open == DeviceGet)    { if (!ReleaseDC(window, device)) Report("error releasedc"); }
	else if (open == DevicePaint)  { EndPaint(window, &paint); }
}

void deviceitem::Font(HFONT newfont)
{
	// takes a font
	// loads it into the device
	// returns nothing

	// LOAD THE FONT INTO THE DEVICE, KEEPING THE FIRST ONE THAT COMES OUT
	HFONT outfont;
	outfont = (HFONT)SelectObject(device, newfont);
	if (!font) font = outfont;
}

void deviceitem::Background(int newbackground)
{
	// takes a background mode
	// loads it into the device
	// returns nothing

	// LOAD THE BACKGROUND MODE INTO THE DEVICE, KEEPING THE FIRST ONE THAT COMES OUT
	int outbackground;
	outbackground = SetBkMode(device, newbackground);
	if (!replacebackground) { replacebackground = true; background = outbackground; }
}

void deviceitem::FontColor(COLORREF newcolor)
{
	// takes a text color
	// loads it into the device
	// returns nothing

	// LOAD THE TEXT COLOR INTO THE DEVICE, KEEPING THE FIRST ONE THAT COMES OUT
	COLORREF outcolor;
	outcolor = SetTextColor(device, newcolor);
	if (!replacefontcolor) { replacefontcolor = true; fontcolor = outcolor; }
}

void deviceitem::BackgroundColor(COLORREF newcolor)
{
	// takes a background color
	// loads it into the device
	// returns nothing

	// LOAD THE TEXT COLOR INTO THE DEVICE, KEEPING THE FIRST ONE THAT COMES OUT
	COLORREF outcolor;
	outcolor = SetBkColor(device, newcolor);
	if (!replacebackgroundcolor) { replacebackgroundcolor = true; backgroundcolor = outcolor; }
}

brushitem BrushColor(COLORREF color)
{
	// takes a color
	// creates a brush of that color
	// returns a brush that must be deleted, or null if any error

	// CREATE A BRUSH OF THE SOLID COLOR
	brushitem brush;
	brush.color = color;
	brush.brush = CreateSolidBrush(color);
	if (!brush.brush) Report("brushcolor: error createsolidbrush");

	// RETURN THE BRUSH COLOR AND HANDLE
	return(brush);
}

HFONT CreateFont(read face, int points)
{
	// takes a font face name, point size, and underline style
	// creates the font
	// returns a handle to the font

	// CREATE THE FONT
	LOGFONT info;
	info.lfHeight         = -points;                      // POINTS, MINUS SIGN REQUIRED
	info.lfWidth          = 0;                            // DEFAULT WIDTH
	info.lfEscapement     = 0;                            // NOT ROTATED
	info.lfOrientation    = 0;                            // NOT ROTATED
	info.lfWeight         = FW_NORMAL;                    // NORMAL, NOT BOLD
	info.lfItalic         = (BYTE)false;                  // NOT ITALIC
	info.lfUnderline      = (BYTE)false;                  // UNDERLINE
	info.lfStrikeOut      = (BYTE)false;                  // NOT STRIKEOUT
	info.lfCharSet        = ANSI_CHARSET;                 // USE ANSI CHARACTERS
	info.lfOutPrecision   = OUT_DEFAULT_PRECIS;           // USE DEFAULT SIZE PRECISION
	info.lfClipPrecision  = CLIP_DEFAULT_PRECIS;          // USE DEFAULT CLIPPING BEHAVIOR
	info.lfQuality        = ANTIALIASED_QUALITY;          // USE ANTIALIASING IF POSSIBLE
	info.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; // ONLY USED IF THE FONT BY NAME IS UNAVAILIABLE
	lstrcpy(info.lfFaceName, face);                       // FONT NAME
	HFONT font;
	font = CreateFontIndirect(&info);
	if (!font) Report("error createfontindirect");
	return(font);
}

void PaintText(deviceitem *device, read r, sizeitem size, bool horizontal, bool vertical, bool left, bool right, int adjust, HFONT font, brushitem *color, brushitem *background)
{
	// takes a device context that has a font loaded into it, text, position and bounding size, and formatting options
	// fills the size and paints the text with an ellipsis
	// returns nothing

	// PREPARE THE DEVICE CONTEXT
	if (font)       device->Font(font);
	if (color)      device->FontColor(color->color);
	if (background) device->BackgroundColor(background->color);

	// FIND OUT HOW BIG THE TEXT WILL BE WHEN PAINTED
	sizeitem text;
	text = SizeText(device, r);

	// IF ONLY A POSITION WAS PROVIDED, PUT IN THE NECESSARY SIZE
	if (size.w <= 0) size.w = text.w;
	if (size.h <= 0) size.h = text.h;

	// DEFINE SPACE ONCE IN A LOCAL VARIABLE
	int space;
	space = 4;

	// ADD MARGINS
	sizeitem bound;
	bound = size;
	if (left) bound.ShiftLeft(space);
	if (right) bound.w -= space;
	bound.Check();

	// MAKE TEXT SMALL ENOUGH SO IT WILL FIT WITHIN BOUND
	if (text.w > bound.w) text.w = bound.w;
	if (text.h > bound.h) text.h = bound.h;

	// POSITION THE TEXT WITHIN BOUND
	text.x = bound.x;
	text.y = bound.y;
	if (horizontal && text.w < bound.w) text.x += ((bound.w - text.w) / 2);
	if (vertical   && text.h < bound.h) text.y += ((bound.h - text.h) / 2);

	// ADJUST THE TEXT IF DOING SO DOESN'T PLACE IT OUTSIDE THE BOUND
	if (text.x + adjust >= bound.x && text.Right() + adjust <= bound.Right()) text.x += adjust;

	// FILL THE BACKGROUND, THIS WON'T MAKE DRAW TEXT'S FLICKER WORSE
	if (background) PaintFill(device, size, background->brush);

	// PAINT THE TEXT
	RECT rectangle;
	if (text.Is()) {

		// DRAW TEXT PAINTS BACKGROUND BENEATH THE TEXT AND THEN TEXT OVER IT, CREATING A FLICKER
		rectangle = text.Rectangle();
		if (!DrawText(device->device, r, -1, &rectangle, DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE)) Report("error drawtext");
	}

	// PUT BACK THE DEVICE CONTEXT
	if (background) device->BackgroundColor(device->backgroundcolor);
	if (color)      device->FontColor(device->fontcolor);
}

sizeitem SizeText(deviceitem *device, read r)
{
	// takes a device context with a font loaded inside, and text
	// determines how wide and high in pixels the text painted will be
	// returns the size width and height, or zeroes if any error

	// GET THE PIXEL DIMENSIONS OF TEXT WRITTEN IN THE LOADED FONT
	SIZE size;
	if (!GetTextExtentPoint32(device->device, r, length(r), &size)) {

		Report("sizedevice: error gettextextentpoint32");
		size.cx = size.cy = 0;
	}

	// RETURN THE SIZE, WILL BE ALL 0 FOR BLANK TEXT
	sizeitem s(size);
	return(s);
}

void PaintFill(deviceitem *device, sizeitem size, HBRUSH brush)
{
	// takes a device context, size, and brush
	// uses the device context to paint the rectangle with the brush
	// returns nothing

	// MAKE SURE THERE IS A SIZE TO FILL
	if (!size.Is()) return;

	// PAINT THE RECTANGLE
	RECT rectangle;
	rectangle = size.Rectangle();
	FillRect(device->device, &rectangle, brush); // WILL RETURN ERROR IF WINDOWS IS LOCKED
}
