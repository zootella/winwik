
// PROGRAM COMPILE SETTINGS
#define PROGRAMTEST false
#define PROGRAMNAME "Winwik"
#define PROGRAMTITLE "Winwik"

// PROGRAM CONSTANTS
#define SAFETY     8                // MAKES EACH LOCAL BUFFER 8 BYTES LONGER AND MAKES THEM EASY TO FIND
#define FOLDERSIZE 94371840         // 90 MB
#define DISCSIZE   (FOLDERSIZE * 7) // 630 MB

// FUNCTION REPLACEMENTS
#define length lstrlen
#define blank make()

// STRING TYPES
typedef CString string;    // STRING IS A CSTRING OBJECT
typedef LPCTSTR read;      // READ IS A POINTER TO CHARACTERS AND A NULL TERMINATOR THAT WILL ONLY BE READ
typedef LPTSTR  write;     // WRITE IS A POINTER TO A CHARACTER BUFFER
typedef TCHAR   character; // CHARACTER IS A CHARACTER BUFFER

// STRING FUNCTION SWITCHES
enum direction {Forward,   Reverse};  // DEFAULT FORWARD AND THE START, OR REVERSE AND THE END
enum matching  {Different, Matching}; // DEFAULT CASE SENSITIVE, OR CASE INSENSITIVE MATCHING

// RENAME ITEM
class renameitem {
public:
renameitem();

	// POINTERS
	class renameitem *back[4], *next[4]; // THE NEXT AND PREVIOUS ITEMS IN 4 SORT ORDERS
	class renameitem *last;              // THE NEWEST ITEM IN THE FOLDER

	// DATA
	DWORD    id;        // FILE IDENTIFICATION NUMBER
	string   name, ext; // RENAMED FILE NAME AND EXTENSION
	DWORD    size;      // DATA SIZE
	FILETIME date;      // DATE MODIFIED
	int      number;    // 0 NOT SET, -1 DUPLICATE TO SEPARATE, OR 1+ NUMBER FOR PARENTHESIS
	int      folder;    // FOLDER NUMBER
};

// GALLERY ITEM
class galleryitem {
public:
galleryitem();

	// POINTERS
	class galleryitem *next;
	class galleryitem *back;

	// DATA
	string name;  // THE FILE NAME, LIKE "image"
	string ext;   // THE FILE EXTENSION, LIKE "jpg"
	string group; // THE SUBFOLDER GROUP, LIKE "" FOR NONE OR "1" FOR THE FOLDER NAME
};

// HANDLE
struct handletop {

	// PROGRAM
	HINSTANCE instance; // Handle.instance INSTANCE HANDLE
	HANDLE heap;        // Handle.heap     PROCESS MEMORY HEAP

	// WINDOWS
	HWND window; // Handle.window MAIN WINDOW
	HWND dialog; // Handle.dialog AN OPENED DIALOG BOX

	// BRUSHES AND FONTS
	brushitem blue, lightblue, yellow, lightyellow, green, lightgreen, red, lightred;
	HFONT arial;
};

// FUNCTIONS IN WINDOW.CPP
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
void DialogUpdate();
LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

// FUNCTIONS IN CORE FILE.CPP
string FileMove(read path, read folder, read name, read ext, int action, int force, int attributes);
bool FileShortcut(read path, read target, read description);
bool FileSame(read path1, read path2);
bool FileFolder(read path, bool file);
int FileLook(read path);
bool FileDelete(read path);
bool FileSet(read path, DWORD attributes);
DWORD FileReadOpen(read path, HANDLE *file, HANDLE *map, LPVOID *view);
bool FileReadClose(HANDLE file, HANDLE map, LPVOID view);
HANDLE FileWriteOpen(read path, int attributes);
bool FileWriteText(HANDLE file, read r);
bool FileWrite(HANDLE file, LPVOID memory, DWORD size);
bool FileWriteClose(HANDLE file);
bool FileCheckPath(read path);
bool FileList(read folder, read search, HANDLE *findfile, string *path);
void FileRun(read path, read parameters);
string FileBrowse(read display);
string FileChoose();
string ListFolders(read folder);

// FUNCTIONS IN CORE MEMORY.CPP
bool CloseHandleSafely(HANDLE h);
bool KillTimerSafely(HWND window, int timer);
bool Message(read title, read message, UINT type);
void Report(read r);
bool CriticalBox(read r);
LPVOID MemoryAllocate(int size);
void MemoryFree(LPVOID memory);

// FUNCTIONS IN CORE STRING.CPP
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

// FUNCTIONS IN CORE TEXT.CPP
string TextDialog(HWND dialog, int control);
string TextWindow(HWND window);
string TextGuid();

// FUNCTIONS IN CORE CLIPBOARD.CPP
void ClipboardCopy(read r);

// FUNCTIONS IN CORE REGISTRY.CPP
bool RegistryReadNumber(HKEY root, read path, read name, int *i);
bool RegistryReadText(HKEY root, read path, read name, string *s);
bool RegistryWriteNumber(HKEY root, read path, read name, int i);
bool RegistryWriteText(HKEY root, read path, read name, read r);
bool RegistryDelete(HKEY root, read path, read name);
HKEY RegistryOpen(HKEY root, read path);
bool RegistryClose(HKEY key);

// FUNCTIONS IN CORE PAINT.CPP
brushitem BrushColor(COLORREF color);
HFONT CreateFont(read face, int points);
void PaintText(deviceitem *device, read r, sizeitem size, bool horizontal, bool vertical, bool left, bool right, int adjust, HFONT font, brushitem *color, brushitem *background);
sizeitem SizeText(deviceitem *device, read r);
void PaintFill(deviceitem *device, sizeitem size, HBRUSH brush);

// FUNCTIONS IN TOOLS.CPP
void List();
void ListFolder(read folder, string *list);
bool FileMoveSimple(read source, read destination, bool folder, bool copy);
void Slice();
void SliceFolder(read base, read folder, int *number, int *size);
void Zero();
void ZeroAdd(read path);

// functions in (test).cpp
void Test();
