
// Window
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, PSTR command, int show);
LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// String
string make(read r1 = "", read r2 = "", read r3 = "", read r4 = "", read r5 = "", read r6 = "", read r7 = "", read r8 = "", read r9 = ""); // Has defaults
string upper(read r);
string lower(read r);
int number(read r);
string numerals(int number, int width = 0); // Has defaults
bool is(read r);
bool isblank(read r);
bool same(read r1, read r2, matching m = Different); // Has defaults
int compare(read r1, read r2, matching m = Different); // Has defaults
bool starts(read r, read t, matching m = Different); // Has defaults
bool trails(read r, read t, matching m = Different); // Has defaults
bool has(read r, read t, matching m = Different); // Has defaults
int find(read r, read t, direction d = Forward, matching m = Different); // Has defaults
string parse(read r, read t1, read t2, matching m = Different); // Has defaults
string before(read r, read t, direction d = Forward, matching m = Different); // Has defaults
string after(read r, read t, direction d = Forward, matching m = Different); // Has defaults
void split(read r, read t, string *b, string *a, direction d = Forward, matching m = Different); // Has defaults
string replace(read r, read t1, read t2, matching m = Different); // Has defaults
string clip(read r, int startindex, int characters = -1); // Has defaults
string on(read r, read t, direction d = Forward, matching m = Different); // Has defaults
string off(read r, read t, direction d = Forward, matching m = Different); // Has defaults
string trim(read r, read t1 = "", read t2 = "", read t3 = ""); // Has defaults

// Utility
void error(read r1 = "", read r2 = "", read r3 = "", read r4 = "", read r5 = "", read r6 = "", read r7 = "", read r8 = "", read r9 = ""); // Has defaults
void log(read r1 = "", read r2 = "", read r3 = "", read r4 = "", read r5 = "", read r6 = "", read r7 = "", read r8 = "", read r9 = ""); // Has defaults
void report(read r);
string FileOpen(read path);
bool FileSave(read path, read r);
void ClipboardJoin();
void ClipboardLeave();
bool ClipboardPaste(string *s);

// Workbench
void ClipboardChanged();

// Test
void Test();


std::vector<string> lines(read r);
void print(std::vector<string> v);

