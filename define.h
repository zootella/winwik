
// Build settings
#define PROGRAM_TEST true
#define PROGRAM_NAME "Winwik"

// Factory presets
#define WRAP_WIDTH 80

// Function renames
#define length lstrlen

// String types
typedef CString string;    // string is a CString object
typedef LPCTSTR read;      // read is a pointer to characters and a null terminator that will only be read
typedef LPTSTR  write;     // write is a pointer to a character buffer
typedef TCHAR   character; // character is a character buffer

// String function switches
enum direction {Forward,   Reverse};  // Default forward and the start, or reverse and the end
enum matching  {Different, Matching}; // Default case sensitive, or case insensitive matching

// Handles
struct handletop {

	HINSTANCE instance; // Running instance
	HWND window; // Main window
	HWND viewer; // Handle to the window of the next program in the clipboard viewer chain
};
