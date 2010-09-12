
// SIZE ITEM
class sizeitem {
public:

	// COORDINATES FROM CLIENT AREA ORIGIN AND WIDTH AND HEIGHT DIMENSIONS
	int x, y, w, h;

	// NEW
	sizeitem() { Clear(); }
	sizeitem(POINT p) { Set(p); }
	sizeitem(RECT  r) { Set(r); }
	sizeitem(SIZE  s) { Set(s); }

	// CLEAR
	void Clear() { x = y = w = h = 0; }

	// SET
	void Set(POINT p) { x = p.x;    y = p.y;   w = 0;           h = 0;            }
	void Set(RECT  r) { x = r.left; y = r.top; w = r.right - x; h = r.bottom - y; }
	void Set(SIZE  s) { x = 0;      y = 0;     w = s.cx;        h = s.cy;         }

	// CONVERT
	POINT Point() { POINT p; p.x = x; p.y = y; return(p); }
	RECT Rectangle() { RECT r; r.left = x; r.top = y; r.right = x + w; r.bottom = y + h; return(r); }
	SIZE Size() { SIZE s; s.cx = w; s.cy = h; return(s); }

	// TAKE NEGATIVE WIDTH OR HEIGHT TO 0
	void Check() { if (w < 0) w = 0; if (h < 0) h = 0; }

	// DETERMINE IF THE SIZE HOLDS ANY PIXELS, AND IF A POINT IS INSIDE THE SIZE
	bool Is() { return(w > 0 && h > 0); }
	bool Inside(sizeitem s) { return(s.x >= x && s.x < x + w && s.y >= y && s.y < y + h); }

	// READ LIKE A RECTANGLE
	int Right() { return(x + w); }
	int Bottom() { return(y + h); }

	// SET LIKE A RECTANGLE
	void SetLeft(int left) { w += x - left; x = left; }
	void SetTop(int top) { h += y - top; y = top; }
	void SetRight(int right) { w = right - x; }
	void SetBottom(int bottom) { h = bottom - y; }

	// POSITION BY THE RIGHT OR BOTTOM EDGES
	void PositionRight(int right) { x = right - w; }
	void PositionBottom(int bottom) { y = bottom - h; }

	// SHIFT JUST THE LEFT AND TOP BOUNDARIES BY A PIXEL AMOUNT
	void ShiftLeft(int shift) { x += shift; w -= shift; }
	void ShiftTop(int shift) { y += shift; h -= shift; }

	// COLLAPSE THE SIZE TO THE RIGHT OR TO THE BOTTOM
	void CloseRight() { x += w; w = 0; }
	void CloseBottom() { y += h; h = 0; }

	// EXPAND ALL THE EDGES BY A PIXEL AMOUNT
	void Expand(int shift = 1) { x -= shift; y -= shift; w += (2 * shift); h += (2 * shift); }

	// CONVERT BETWEEN SCREEN AND CLIENT WINDOW COORDINATES
	void Screen(HWND window = NULL);
	void Client(HWND window = NULL);
};

// DEVICE ITEM
enum deviceopen {

	DeviceNone,   // THE DEVICE CONTEXT HAS NOT BEEN OBTAINED YET
	DeviceUse,    // THE SYSTEM PROVIDED A DEVICE CONTEXT FOR THE PROGRAM TO USE
	DeviceCreate, // THE PROGRAM CREATED A DEFAULT DISPLAY DEVICE CONTEXT
	DeviceGet,    // THE PROGRAM ASKED THE SYSTEM FOR THE WINDOW'S DEVICE CONTEXT
	DevicePaint,  // THE PROGRAM GOT THE DEVICE CONTEXT IN RESPONSE TO A PAINT MESSAGE
};
class deviceitem {
public:
~deviceitem();

	// METHODS
	void OpenUse(HDC newdevice);
	void OpenCreate();
	void OpenGet(HWND newwindow);
	void OpenPaint(HWND newwindow);
	void Font(HFONT newfont);
	void Background(int newbackground);
	void FontColor(COLORREF newcolor);
	void BackgroundColor(COLORREF newcolor);

	// THE DEVICE CONTEXT
	deviceopen open;
	HDC device;
	HWND window;
	PAINTSTRUCT paint;

	// PAINTING TOOLS TO PUT BACK
	HFONT font;
	bool replacebackground, replacefontcolor, replacebackgroundcolor;
	COLORREF fontcolor, backgroundcolor;
	int background;

	// NEW
	deviceitem() {

		open = DeviceNone;
		device = NULL;
		window = NULL;
		font = NULL;
		replacebackground = replacefontcolor = replacebackgroundcolor = false;
	}
};

// BRUSH ITEM
class brushitem {
public:

	// COLOR AND BRUSH
	COLORREF color;
	HBRUSH brush;

	// NEW
	brushitem() {

		brush = NULL;
	}
};
