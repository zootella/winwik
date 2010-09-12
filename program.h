
// Build settings
#define PROGRAM_TEST false
#define PROGRAM_NAME "Winwik"

// Constants
#define SAFETY 8

// Function renames
#define length lstrlen

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

// Functions in core memory.cpp
bool CloseHandleSafely(HANDLE h);
void Report(read r);

// Functions in core string.cpp
CString make(read r1 = "", read r2 = "", read r3 = "", read r4 = "", read r5 = "", read r6 = "", read r7 = "", read r8 = "", read r9 = ""); // Has defaults
CString upper(read r);
CString lower(read r);
int number(read r);
CString numerals(int number, int width = 0); // Has defaults
bool is(read r);
bool isblank(read r);
bool same(read r1, read r2, matching m = Different); // Has defaults
int compare(read r1, read r2, matching m = Different); // Has defaults
bool starts(read r, read t, matching m = Different); // Has defaults
bool trails(read r, read t, matching m = Different); // Has defaults
bool has(read r, read t, matching m = Different); // Has defaults
int find(read r, read t, direction d = Forward, matching m = Different); // Has defaults
CString parse(read r, read t1, read t2, matching m = Different); // Has defaults
CString before(read r, read t, direction d = Forward, matching m = Different); // Has defaults
CString after(read r, read t, direction d = Forward, matching m = Different); // Has defaults
void split(read r, read t, CString *b, CString *a, direction d = Forward, matching m = Different); // Has defaults
CString replace(read r, read t1, read t2, matching m = Different); // Has defaults
CString clip(read r, int startindex, int characters = -1); // Has defaults
CString on(read r, read t, direction d = Forward, matching m = Different); // Has defaults
CString off(read r, read t, direction d = Forward, matching m = Different); // Has defaults
CString trim(read r, read t1 = "", read t2 = "", read t3 = ""); // Has defaults

// Test functions
void Test();
