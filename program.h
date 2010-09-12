
// Build settings
#define PROGRAM_TEST false
#define PROGRAM_NAME "Winwik"

// Constants
#define SAFETY 8

// Function renames
#define length lstrlen
#define blank make()

// String types
typedef CString string;
typedef LPCTSTR read;
typedef LPTSTR  write;
typedef TCHAR   character;

// String function switches
enum direction {Forward,   Reverse};  // Default forward and the start, or reverse and the end
enum matching  {Different, Matching}; // Default case sensitive, or case insensitive matching

// Handles
struct handletop {

	HINSTANCE instance; // Running instance
	HWND window; // Main window
};

// Functions in window.cpp
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

// Functions in core file.cpp

// Functions in core memory.cpp
bool CloseHandleSafely(HANDLE h);
void Report(read r);

// Functions in core string.cpp
string make(read r1 = "", read r2 = "", read r3 = "", read r4 = "", read r5 = "", read r6 = "", read r7 = "", read r8 = "", read r9 = "");
string upper(read r);
string lower(read r);
int number(read r);
string numerals(int number);
bool is(read r);
bool isblank(read r);
bool same(read r1, read r2, matching m = Different);
int compare(read r1, read r2, matching m = Different);
bool starts(read r, read t, matching m = Different);
bool trails(read r, read t, matching m = Different);
bool has(read r, read t, matching m = Different);
int find(read r, read t, direction d = Forward, matching m = Different);
string parse(read r, read t1, read t2, matching m = Different);
string before(read r, read t, direction d = Forward, matching m = Different);
string after(read r, read t, direction d = Forward, matching m = Different);
void split(read r, read t, string *b, string *a, direction d = Forward, matching m = Different);
string replace(read r, read t1, read t2, matching m = Different);
string clip(read r, int startindex, int characters);
string on(read r, read t, direction d = Forward, matching m = Different);
string off(read r, read t, direction d = Forward, matching m = Different);
string trim(read r, read t1 = "", read t2 = "", read t3 = "");
string saynumber(int number, read name);
string insertcommas(read r);

// Functions in core text.cpp
string TextDialog(HWND dialog, int control);
string TextWindow(HWND window);
string TextGuid();

// Functions in core clipboard.cpp

// Functions in tools.cpp
void List();
void ListFolder(read folder, string *list);
bool FileMoveSimple(read source, read destination, bool folder, bool copy);

// Test functions
void Test();
