
// Include platform
#include <windows.h>
#include <windef.h>
#include <atlstr.h>

// Include language
#include <vector>

// Include program
#include "resource.h"
#include "define.h"
#include "function.h"

// Access to global objects
extern handletop Handle;

// Takes text
// Copies and catcatenates the text into a string
// Returns a string
string make(read r1, read r2, read r3, read r4, read r5, read r6, read r7, read r8, read r9) {

	string s1 = r1, s2 = r2, s3 = r3, s4 = r4, s5 = r5, s6 = r6, s7 = r7, s8 = r8, s9 = r9;
	return s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9;
}

// Takes text
// Uppercases the characters in it
// Returns a string
string upper(read r) {

	string s = r;
	s.MakeUpper();
	return s;
}

// Takes text
// Lowercases the characters in it
// Returns a string
string lower(read r) {

	string s = r;
	s.MakeLower();
	return s;
}

// Takes text
// Reads the text as a number, handling a leading minus sign properly
// Returns the number, or 0 if given blank or alphabetic text
int number(read r) {

	return atoi(r);
}

// Takes a number and width like 3 for 001
// Writes the minus sign and number into text
// Returns a string
string numerals(int number, int width) {

	WCHAR bay[MAX_PATH];
	_itow_s(number, bay, MAX_PATH, 10); // The 10 is for base ten
	string s = bay;

	if (width) {
		while (length(s) < width)
			s = "0" + s; // Loop until we've put enough 0s at the start
	}

	return s;
}

// Takes text
// Determines if the text is not blank
// Returns true if it is, false if not
bool is(read r) {

	if (r[0] != '\0') return true; // The text doesn't begin with a null terminator, and isn't blank
	else              return false;
}

// Takes text
// Determines if the text is blank
// Returns true if it is, false if not
bool isblank(read r) {

	if (r[0] == '\0') return true; // The text begins with the null terminator, and is blank
	else              return false;
}

// Takes text r1 and r2, and matching
// Determines if r1 and r2 are the same
// Returns true if they are, false if they are not
bool same(read r1, read r2, matching m) {

	if (compare(r1, r2, m) == 0) return true;  // They are the same
	else                         return false;
}

// Takes text r1 and r2, and matching
// Calls lstrcmp or lstrcmpi on them
// Returns the result, which is negative if r1 is before r2, zero if they are the same, and positive if r1 is below r2
int compare(read r1, read r2, matching m) {

	if (m == Different) return lstrcmp(r1, r2);  // Case sensitive, the default
	else                return lstrcmpi(r1, r2); // Case insensitive, matching cases
}

// Takes text r and t, and matching
// Determins if the text starts with the tag
// Returns true if it does, false if it does not or if r or t are blank
bool starts(read r, read t, matching m) {

	// Use find to determine if the tag is at the start of the text
	if (find(r, t, Forward, m) == 0) return true;
	else                             return false;
}

// Takes text r and t, and matching
// Determins if the text ends with the tag
// Returns true if it does, false if it does not or if r or t are blank
bool trails(read r, read t, matching m) {

	// Find the last instance of the tag
	int result;
	result = find(r, t, Reverse, m);
	if (result == -1) return false; // Tag not found

	if (result == length(r) - length(t)) return true;  // Tag found on end
	else                                 return false; // Tag found elsewhere
}

// Takes text r and t, and matching
// Determins if the tag appears in the text
// Returns true if it does, false if it does not or if r or t are blank
bool has(read r, read t, matching m) {

	// Use find to determine if the tag exists in the text
	if (find(r, t, Forward, m) != -1) return true;
	else                              return false;
}

// Takes text r and t, and direction and matching
// Finds in r the first or last instance of t
// Returns the zero based index of t in r, or -1 if not found or if r or t are blank
int find(read r, read t, direction d, matching m) {

	// Get lengths
	int rlength, tlength;
	rlength = length(r);
	tlength = length(t);

	// If either is blank or r is shorter than t, return not found
	if (!rlength || !tlength || rlength < tlength) return -1;

	// Variables for loop
	bool valid;         // Valid tells if the tag is being found
	int rindex, tindex; // Scannign indices
	WCHAR rchar, tchar; // Characters

	// Scan rindex between 0 and rlength - tlength in the desired direction
	if (d == Forward) rindex = 0;
	else              rindex = rlength - tlength;
	while (1) {
		if (d == Forward) { if (rindex > rlength - tlength) break; }
		else              { if (rindex < 0)                 break; }

		// Set valid true and look for the tag at rindex, to either break false at first mismatch or finish true
		valid = true;
		for (tindex = 0; tindex <= tlength - 1; tindex++) {

			// Get the pair of characters
			rchar = r[rindex + tindex];
			tchar = t[tindex];

			// Uppercase them if matching was requested
			if (m == Matching) {

				rchar = (WCHAR)CharUpper((LPTSTR)(ULONG_PTR)MAKELONG((WORD)rchar, 0));
				tchar = (WCHAR)CharUpper((LPTSTR)(ULONG_PTR)MAKELONG((WORD)tchar, 0));
			}

			// Mismatch found, set false and break
			if (rchar != tchar) { valid = false; break; }
		}

		// The tag was found at rindex, return it, done
		if (valid) return rindex;

		if (d == Forward) rindex++;
		else              rindex--;
	}

	// Not found
	return -1;
}

// Takes text to parse, opening and closing tags, and matching
// Gets the text between the tags
// Returns a string
string parse(read r, read t1, read t2, matching m) {

	// Clip from after the first tag and then before the second or blank if not found
	string s = after(r, t1, Forward, m);
	if (has(s, t2, m)) s = before(s, t2, Forward, m);
	else               s = "";
	return s;
}

// Takes text and tag, and direction and matching
// Splits the text before the tag
// Returns a string, the text from r if not found in either direction
string before(read r, read t, direction d, matching m) {

	// Use split
	string b, a;
	split(r, t, &b, &a, d, m);
	return b;
}

// Takes text and tag, and direction and matching
// Splits the text after the tag
// Returns a string, blank if not found in either direction
string after(read r, read t, direction d, matching m) {

	// Use split
	string b, a;
	split(r, t, &b, &a, d, m);
	return a;
}

// Takes text and tag, strings for before and after, and direction and matching
// Splits the text around the tag, writing text in before and after
// Returns nothing, puts all text in before and none in after if not found in either direction
void split(read r, read t, string *b, string *a, direction d, matching m) {

	// Find the tag in the text using the direction and matching passed to this function
	int i;
	i = find(r, t, d, m);
	if (i == -1) {

		// Not found, all text is before and none is after, done
		*b = r;
		*a = "";
		return;
	}

	// Get lengths
	int rlength, tlength;
	rlength = length(r);
	tlength = length(t);

	// Clip out before and after form a copy of r so that r and *b being the same won't mangle *a
	string source = r;
	*b = clip(source, 0, i);
	*a = clip(source, i + tlength, rlength - tlength - i);
}

// Takes text, find and replace tags, and matching
// Makes a single pass down the text, replacing whole instances of the find text with the replacement text
// Returns a string
string replace(read r, read t1, read t2, matching m) {

	// If the text or the find text is blank, or if the find text is not found, return the text unchanged
	string top, left, bottom;
	top = r;
	if (isblank(r) || isblank(t1) || !has(r, t1, m)) return top;

	// Loop while top has find
	while (has(top, t1, m)) {

		// f is in top
		split(top, t1, &left, &top, Forward, m);
		bottom += left + t2;
	}

	// f is not in top
	bottom += top;

	// Return bottom text
	return bottom;
}

// Takes text, a starting index, and a number of characters to copy or -1 for all
// Clips out that text, not reading outside of r
// Returns a string
string clip(read r, int startindex, int characters) {

	// Get the length and eliminate special cases
	string s;
	int n = length(r);
	if (n == 0 || characters == 0) { return s; }            // No characters to clip or none requested
	if (startindex < 0 || startindex > n - 1) { return s; } // Start index outside of r

	// Adjust local copy of characters
	if (characters < 0 || characters > n - startindex) characters = n - startindex;

	// Copy the text into the string, crop it, and return it
	s = r;
	s = s.Mid(startindex, characters);
	return s;
}

// Takes text and tag, and direction and matching
// Confirms the text starts or ends with the tag, inserting it if necessary
// Returns a string
string on(read r, read t, direction d, matching m) {

	string s = r;
	if (d == Forward) { if (!starts(s, t, m)) s = t + s; } // Confirm the text starts with the tag
	else              { if (!trails(s, t, m)) s = s + t; } // Confirm the text ends with the tag
	return s;
}

// Takes text and tag, and direction and matching
// Confirms the text does not start or end with the tag, removing multiple instances of it if necessary
// Returns a string
string off(read r, read t, direction d, matching m) {

	string s = r;
	if (d == Forward) { while(starts(s, t, m)) s = clip(s, length(t), -1); }            // Remove the tag from the start of the string
	else              { while(trails(s, t, m)) s = clip(s, 0, length(s) - length(t)); } // Remove the tag from the end of the string
	return s;
}

// Takes text and tags
// Removes the tags from the start and end of the text
// Returns a string
string trim(read r, read t1, read t2, read t3) {

	// Copy the text into a string
	string s = r;

	// Remove the tags from the start of the string until gone
	while (true) {

		if      (starts(s, t1)) s = clip(s, length(t1), -1);
		else if (starts(s, t2)) s = clip(s, length(t2), -1);
		else if (starts(s, t3)) s = clip(s, length(t3), -1);
		else                    break;
	}

	// Remove the tags from the end of the string until gone
	while (true) {

		if      (trails(s, t1)) s = clip(s, 0, length(s) - length(t1));
		else if (trails(s, t2)) s = clip(s, 0, length(s) - length(t2));
		else if (trails(s, t3)) s = clip(s, 0, length(s) - length(t3));
		else                    break;
	}

	// Return the string
	return s;
}
