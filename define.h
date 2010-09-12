
// Build settings
#define PROGRAM_TEST true
#define PROGRAM_NAME "Winwik"

// Constants
#define SAFETY 8

// Function renames
#define length lstrlen

// String types
typedef CString string;
typedef LPCTSTR read;

// String function switches
enum direction {Forward,   Reverse};  // Default forward and the start, or reverse and the end
enum matching  {Different, Matching}; // Default case sensitive, or case insensitive matching

// Handles
struct handletop {

	HINSTANCE instance; // Running instance
	HWND window; // Main window
	HWND viewer; // Handle to the window of the next program in the clipboard viewer chain
};
