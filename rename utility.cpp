
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
extern statetop  State;  // STATE VARIABLES

renameitem::renameitem()
{
	// POINTERS
	back[0] = back[1] = back[2] = back[3] = NULL;
	next[0] = next[1] = next[2] = next[3] = NULL;
	last = NULL;

	// DATA
	id     = 0;
	size   = 0;
	number = 0;
	folder = 0;
}

void ComposeName(read path, string *name, string *ext)
{
	// takes a file path
	// composes the renamed file name and extension for the file
	// returns nothing, writes the name and extension text

	// CLIP THE NAME AND EXTENSION FROM THE PATH
	split(after(path, "\\", Reverse, Different), ".", name, ext, Reverse, Different);

	// LOWERCASE THE NAME AND REMOVE ALL SPACES
	*name = replace(lower(*name), " ", "");

	// REMOVE ANY UNTITLED TEXT FROM THE NAME
	while (has(*name, "(untitled)")) *name = replace(*name, "(untitled)", "");

	// REMOVE A NUMBER IN PARENTHESIS AT THE START OF THE NAME
	string inside, outside;
	int n, i;
	bool alldigits;
	if (starts(*name, "(") && has(*name, ")")) {

		// NAME STARTS WITH A PARENTHESIS AND CONTAINS A CLOSING PARENTHESIS, SPLIT OUT THE TWO PARTS INSIDE
		split(*name, ")", &inside, &outside);
		inside = off(inside, "(");

		// DETERMINE IF THE TEXT INSIDE PARENTHESIS IS ONLY DIGITS
		n = length(inside);
		alldigits = true;
		for (i = 0; i < n; i++) if (inside[i] < '0' || inside[i] > '9') alldigits = false;

		// IF IT HAS LENGTH AND IS ALL DIGITS, MAKE NAME JUST THE OUTSIDE PART
		if (n > 0 && alldigits) *name = outside;
	}

	// REMOVE A NUMBER IN PARENTHESIS AT THE END OF THE NAME
	if (has(*name, "(") && trails(*name, ")")) {

		// NAME CONTAINS A PARENTHESIS AND ENDS WITH A CLOSING PARENTHESIS, SPLIT OUT THE TWO PARTS INSIDE
		split(*name, "(", &outside, &inside, Reverse, Different);
		inside = off(inside, ")", Reverse, Different);

		// DETERMINE IF THE TEXT INSIDE PARENTHESIS IS ONLY DIGITS
		n = length(inside);
		alldigits = true;
		for (i = 0; i < n; i++) if (inside[i] < '0' || inside[i] > '9') alldigits = false;

		// IF IT HAS LENGTH AND IS ALL DIGITS, MAKE NAME JUST THE OUTSIDE PART
		if (n > 0 && alldigits) *name = outside;
	}

	// REMOVE COMMON NUMBERING PATTERNS AT THE END FOR LOW NUMBERS ONLY
	*name = off(*name, "_0", Reverse, Different);
	*name = off(*name, "_1", Reverse, Different);
	*name = off(*name, "_2", Reverse, Different);
	*name = off(*name, "_3", Reverse, Different);
	*name = off(*name, "_4", Reverse, Different);
	*name = off(*name, "_5", Reverse, Different);
	*name = off(*name, "_6", Reverse, Different);
	*name = off(*name, "_7", Reverse, Different);
	*name = off(*name, "_8", Reverse, Different);
	*name = off(*name, "_9", Reverse, Different);
	*name = off(*name, "~0", Reverse, Different);
	*name = off(*name, "~1", Reverse, Different);
	*name = off(*name, "~2", Reverse, Different);
	*name = off(*name, "~3", Reverse, Different);
	*name = off(*name, "~4", Reverse, Different);
	*name = off(*name, "~5", Reverse, Different);
	*name = off(*name, "~6", Reverse, Different);
	*name = off(*name, "~7", Reverse, Different);
	*name = off(*name, "~8", Reverse, Different);
	*name = off(*name, "~9", Reverse, Different);
	*name = off(*name, "[0]", Reverse, Different);
	*name = off(*name, "[1]", Reverse, Different);
	*name = off(*name, "[2]", Reverse, Different);
	*name = off(*name, "[3]", Reverse, Different);
	*name = off(*name, "[4]", Reverse, Different);
	*name = off(*name, "[5]", Reverse, Different);
	*name = off(*name, "[6]", Reverse, Different);
	*name = off(*name, "[7]", Reverse, Different);
	*name = off(*name, "[8]", Reverse, Different);
	*name = off(*name, "[9]", Reverse, Different);

	// REPLACE ACCENTED CHARACTERS WITH NORMAL ONES
	*name = replace(*name, "à", "a");
	*name = replace(*name, "á", "a");
	*name = replace(*name, "â", "a");
	*name = replace(*name, "ã", "a");
	*name = replace(*name, "ä", "a");
	*name = replace(*name, "å", "a");
	*name = replace(*name, "è", "e");
	*name = replace(*name, "é", "e");
	*name = replace(*name, "ê", "e");
	*name = replace(*name, "ì", "i");
	*name = replace(*name, "í", "i");
	*name = replace(*name, "î", "i");
	*name = replace(*name, "ï", "i");
	*name = replace(*name, "ò", "o");
	*name = replace(*name, "ó", "o");
	*name = replace(*name, "ô", "o");
	*name = replace(*name, "õ", "o");
	*name = replace(*name, "ö", "o");
	*name = replace(*name, "ù", "u");
	*name = replace(*name, "ú", "u");
	*name = replace(*name, "û", "u");
	*name = replace(*name, "ü", "u");

	// REMOVE TEXT
	while (has(*name, "copyof")) *name = replace(*name, "copyof", "");
	while (has(*name, ".gif"))   *name = replace(*name, ".gif",   "");
	while (has(*name, "-gif"))   *name = replace(*name, "-gif",   "");
	while (has(*name, "_gif"))   *name = replace(*name, "_gif",   "");
	while (has(*name, ".jpg"))   *name = replace(*name, ".jpg",   "");
	while (has(*name, "-jpg"))   *name = replace(*name, "-jpg",   "");
	while (has(*name, "_jpg"))   *name = replace(*name, "_jpg",   "");
	while (has(*name, ".jpeg"))  *name = replace(*name, ".jpeg",  "");
	while (has(*name, "-jpeg"))  *name = replace(*name, "-jpeg",  "");
	while (has(*name, "_jpeg"))  *name = replace(*name, "_jpeg",  "");
	while (has(*name, "www."))   *name = replace(*name, "www.",   "");
	while (has(*name, "www-"))   *name = replace(*name, "www-",   "");
	while (has(*name, "www_"))   *name = replace(*name, "www_",   "");
	while (has(*name, ".com"))   *name = replace(*name, ".com",   "");
	while (has(*name, "-com"))   *name = replace(*name, "-com",   "");
	while (has(*name, "_com"))   *name = replace(*name, "_com",   "");

	// TURN "%252525" BUNCHES OF ANY LENGTH INTO SINGLE "%" CHARACTERS
	while (has(*name, "%25")) *name = replace(*name, "%25", "%");

	// REMOVE ALL CODES IN HTTP BOOK, MATCHING CASE FOR HEXIDECIMAL DIGITS
	*name = replace(*name, "%20", "", Matching);
	*name = replace(*name, "%21", "", Matching);
	*name = replace(*name, "%22", "", Matching);
	*name = replace(*name, "%23", "", Matching);
	*name = replace(*name, "%24", "", Matching);
	*name = replace(*name, "%26", "", Matching);
	*name = replace(*name, "%27", "", Matching);
	*name = replace(*name, "%28", "", Matching);
	*name = replace(*name, "%29", "", Matching);
	*name = replace(*name, "%2B", "", Matching);
	*name = replace(*name, "%2C", "", Matching);
	*name = replace(*name, "%2F", "", Matching);
	*name = replace(*name, "%3A", "", Matching);
	*name = replace(*name, "%3B", "", Matching);
	*name = replace(*name, "%3C", "", Matching);
	*name = replace(*name, "%3D", "", Matching);
	*name = replace(*name, "%3E", "", Matching);
	*name = replace(*name, "%3F", "", Matching);
	*name = replace(*name, "%40", "", Matching);
	*name = replace(*name, "%5B", "", Matching);
	*name = replace(*name, "%5C", "", Matching);
	*name = replace(*name, "%5D", "", Matching);
	*name = replace(*name, "%5E", "", Matching);
	*name = replace(*name, "%60", "", Matching);
	*name = replace(*name, "%7B", "", Matching);
	*name = replace(*name, "%7C", "", Matching);
	*name = replace(*name, "%7D", "", Matching);
	*name = replace(*name, "%7E", "", Matching);

	// TURN ANY INTERNAL . INTO -
	*name = replace(*name, ".",  "-");

	// REMOVE ALL CHARACTERS THAT ARENT 0-9 a-z - _
	*name = TextStrip(*name);

	// REMOVE ALL TRAILING AND LEADING - _
	*name = trim(*name, "-", "_");

	// REMOVE RUNS OF - _ TOGETHER
	while (has(*name, "--") ||
		   has(*name, "-_") ||
		   has(*name, "_-") ||
		   has(*name, "__")) {

		*name = replace(*name, "--", "-");
		*name = replace(*name, "-_", "-");
		*name = replace(*name, "_-", "-");
		*name = replace(*name, "__", "-");
	}

	// CHOP OFF AT 32 CHARACTERS
	*name = clip(*name, 0, 32);
	*name = trim(*name, "-", "_");

	// BLANK RESERVED NAMES
	if (same(*name, "aux")   ||
		same(*name, "clock") ||
		same(*name, "com1")  ||
		same(*name, "com2")  ||
		same(*name, "com3")  ||
		same(*name, "com4")  ||
		same(*name, "com5")  ||
		same(*name, "com6")  ||
		same(*name, "com7")  ||
		same(*name, "com8")  ||
		same(*name, "com9")  ||
		same(*name, "con")   ||
		same(*name, "lpt1")  ||
		same(*name, "lpt2")  ||
		same(*name, "lpt3")  ||
		same(*name, "lpt4")  ||
		same(*name, "lpt5")  ||
		same(*name, "lpt6")  ||
		same(*name, "lpt7")  ||
		same(*name, "lpt8")  ||
		same(*name, "lpt9")  ||
		same(*name, "nul")   ||
		same(*name, "prn")) {

		*name = "";
	}

	// LOWERCASE THE EXTENSION, REMOVE ALL CHARACTERS THAT ARENT 0-9 a-z - _ AND CHOP OFF AT 32 CHARACTERS
	*ext = lower(*ext);
	*ext = TextStrip(*ext);
	*ext = clip(*ext, 0, 32);

	// CHANGE JPEG, JPE, AND MPEG TO JPG AND MPG
	if (same(*ext, "jpe"))  *ext = "jpg";
	if (same(*ext, "jpeg")) *ext = "jpg";
	if (same(*ext, "mpeg")) *ext = "mpg";
}

string ComposePathPrepare(renameitem *r)
{
	// takes a rename item
	// composes the prepared temporary hold path for the file
	// returns a string

	// MAKE PATH LIKE "D:\test(done)\(separated)\0\(0)name.ext"
	string path;
	path = State.rename.done + "(separated)\\" + numerals((int)(r->id / 1000)) + "\\(" + numerals(r->id) + ")" + r->name;
	if (is(r->ext)) path += make(".", r->ext);
	return(path);
}

string ComposePathDestination(renameitem *r)
{
	// takes a rename item
	// composes the renamed destination path
	// returns a string

	// MAKE PATH LIKE "D:\test(done)\"
	string path;
	path = State.rename.done;

	// COMPOSE THE FOLDER NAME LIKE "001 May 2001\" OR BLANK IF ERROR OR TOO BIG FOR FOLDER
	if (r->folder > 0 && r->last) {

		// COMPOSE NUMBER TEXT LIKE "001"
		string number, month, year;
		number = numerals(r->folder);
		while (length(number) < 3) number = "0" + number;

		// MAKE WINDOWS TIME CALLS
		FILETIME local;
		FileTimeToLocalFileTime(&r->last->date, &local); // BRING THE FILETIME INTO THE LOCAL SYSTEM TIME ZONE
		SYSTEMTIME date;
		FileTimeToSystemTime(&local, &date);             // BREAK THE RESULTING LOCAL TIME INTO ITS PARTS

		// COMPOSE MONTH NAME
		if (date.wMonth == 1)  month = "January";
		if (date.wMonth == 2)  month = "February";
		if (date.wMonth == 3)  month = "March";
		if (date.wMonth == 4)  month = "April";
		if (date.wMonth == 5)  month = "May";
		if (date.wMonth == 6)  month = "June";
		if (date.wMonth == 7)  month = "July";
		if (date.wMonth == 8)  month = "August";
		if (date.wMonth == 9)  month = "September";
		if (date.wMonth == 10) month = "October";
		if (date.wMonth == 11) month = "November";
		if (date.wMonth == 12) month = "December";

		// COMPOSE YEAR TEXT LIKE "2001"
		year = numerals(date.wYear);

		// MAKE PATH LIKE "D:\test(done)\001 September 2001\"
		path += number + " " + month + " " + year + "\\";
	}

	// MAKE PATH LIKE "D:\test(done)\001 September 2001\name"
	if (is(r->name)) path += r->name;
	else             path += "(untitled)";

	// MAKE PATH LIKE "D:\test(done)\001 September 2001\name(2)"
	if (r->number > 1) path += "(" + numerals(r->number) + ")";

	// MAKE PATH LIKE "D:\test(done)\001 September 2001\name(2).ext"
	if (is(r->ext)) path += make(".", r->ext);

	// RETURN THE PATH
	return(path);
}

void RenameInsert(renameitem *r)
{
	// takes a rename item
	// adds it to the list
	// returns nothing

	// SORT AND LINK R INTO THE LIST
	if (State.rename.r) {

		// LOOP FOR LEVELS 0, 1, 2, AND 3
		renameitem *i;
		int level, tail, sort;
		for (level = 0; level < 4; level++) {

			// START M ON THE LAST ITEM ADDED
			i = State.rename.r;
			tail = 0;
			while (1) {

				// DETERMINE IF R SHOULD BE SORTED BEFORE OR AFTER M IN THE LIST
				sort = RenameSort(i, r, level);
				if (!tail) tail = sort; // SET THE DIRECTION ON THE FIRST LOOP

				if (sort < 0) {

					// MOVED BACK TO GET HERE AND SORT BEFORE, MOVE BACK AGAIN OR PLACE FIRST
					if (tail < 0) {

						if (i->back[level]) i = i->back[level];
						else break;

					// MOVED FORWARD TO GET HERE AND SORT BEFORE, PLACE BEFORE
					} else {

						tail = -1;
						break;
					}

				} else {

					// MOVED BACK TO GET HERE AND SORT AFTER, PLACE AFTER
					if (tail < 0) {

						tail = 1;
						break;

					// MOVED FORWARD TO GET HERE AND SORT AFTER, MOVE FORWARD AGAIN OR PLACE LAST
					} else {

						if (i->next[level]) i = i->next[level];
						else break;
					}
				}
			}

			// INSERT THE NEW ITEM INTO THE LIST
			if (tail < 0) {

				// INSERT BEFORE
				if (i->back[level]) {

					r->back[level] = i->back[level];
					i->back[level]->next[level] = r;
				}

				i->back[level] = r;
				r->next[level] = i;

			} else {

				// INSERT AFTER
				if (i->next[level]) {

					r->next[level] = i->next[level];
					i->next[level]->back[level] = r;
				}

				i->next[level] = r;
				r->back[level] = i;
			}
		}
	}

	// R IS THE ITEM MOST RECENTLY ADDED
	State.rename.r = r;
}

int RenameSort(renameitem *i, renameitem *r, int level)
{
	// takes a rename item in the list, r to add, and the link level
	// determines if r should be listed before or after i
	// returns -1 before or 1 after

	// COMPARE THEM
	int name, date;
	name = lstrcmp(i->name, r->name);
	date = CompareFileTime(&i->date, &r->date);

	// LEVEL 0 FOR GROUPING AND MOVING
	if (level == 0) {

		// DATE NAME SIZE
		if (date > 0)          return(-1);
		if (date < 0)          return(1);
		if (name > 0)          return(-1);
		if (name < 0)          return(1);
		if (i->size > r->size) return(-1);
		if (i->size < r->size) return(1);

	// LEVEL 1 FOR MARKING DUPLICATE DATA
	} else if (level == 1) {

		// SIZE DATE NAME
		if (i->size > r->size) return(-1);
		if (i->size < r->size) return(1);
		if (date > 0)          return(-1);
		if (date < 0)          return(1);
		if (name > 0)          return(-1);
		if (name < 0)          return(1);

	// LEVEL 2 FOR MARKING DUPLICATE NAME AND DATA
	} else if (level == 2) {

		// NAME SIZE DATE
		if (name > 0)          return(-1);
		if (name < 0)          return(1);
		if (i->size > r->size) return(-1);
		if (i->size < r->size) return(1);
		if (date > 0)          return(-1);
		if (date < 0)          return(1);

	// LEVEL 3 FOR NUMBERING
	} else {

		// NAME DATE SIZE
		if (name > 0)          return(-1);
		if (name < 0)          return(1);
		if (date > 0)          return(-1);
		if (date < 0)          return(1);
		if (i->size > r->size) return(-1);
		if (i->size < r->size) return(1);
	}

	// TIE, SORT AFTER FOR ORDER ADDED
	return(1);
}

renameitem *RenameFirst(int level)
{
	// takes a level number
	// finds the first item in the list at that level
	// returns a pointer to the start of the list

	// START BY POINTING R AT THE MOST RECENTLY ADDED ITEM IN THE LIST
	renameitem *r;
	r = State.rename.r;

	// MOVE R BACK ON THE LEVEL AS FAR AS POSSIBLE
	if (r) {

		while (r->back[level]) r = r->back[level];
	}

	// RETURN THE POINTER TO THE START OF THE LIST
	return(r);
}

renameitem *RenameNext(renameitem *r, int level)
{
	// takes a rename item in the list and the level to navigate
	// if on an item with a number, moves to the next item which has no number
	// returns a position in the list

	// MOVE FORWARD IN THE LIST FROM THE CURRENT POSITION
	while (true) {

		// IF R IS OFF THE END OF THE LIST OR HAS NO NUMBER, RETURN IT
		if (!r || !r->number) return(r);
		r = r->next[level];
	}
}

renameitem *RenameGroupNext(renameitem *r)
{
	// takes a rename item in the list
	// if on a group of duplicates and big files, moves beyond it
	// returns a position in the list

	// MOVE FORWARD IN THE LIST FROM THE CURRENT POSITION
	while (true) {

		// IF R IS OFF THE END OF THE LIST OR ON A NORMAL FILE, RETURN IT
		if (!r || (r->number != -1 && r->size <= FOLDERSIZE)) return(r);
		r = r->next[0];
	}
}

bool RenameSameData(renameitem *r1, renameitem *r2)
{
	// takes two rename items
	// compares their data
	// returns true if they have identicle data, false if different

	// IF THE SIZES DON'T MATCH, RETURN DIFFERENT
	if (r1->size != r2->size) return(false);

	// GET THE PATHS
	string path1, path2;
	path1 = ComposePathPrepare(r1);
	path2 = ComposePathPrepare(r2);

	// COMPARE THE FILES AND RETURN THE RESULT
	return(FileSame(path1, path2));
}

int RenameFileLook(read path, FILETIME *date)
{
	// takes a drive or network file, folder, drive letter, or share name path that can have or not have a trailing slash, and null or access to a date
	// determines if the path is already in use by a file or folder, and gets the file size
	// returns -3 error, -2 path availiable, -1 path in use by folder, 0+ path in use by file of that size

	// CHECK THE PATH
	if (!FileCheckPath(path)) return(-3);

	// COPY THE PATH TO A STRING AND REMOVE ANY TRAILING BACKSLASHES
	string s;
	s = path;
	s = off(s, "\\", Reverse, Different);

	// CALL FIND FIRST FILE
	HANDLE findfile;
	WIN32_FIND_DATA info;
	int error;
	findfile = FindFirstFile(s, &info);
	error = GetLastError();

	// THE SEARCH FAILED
	if (findfile == INVALID_HANDLE_VALUE) {

		// CLOSE THE SEARCH
		FindClose(findfile);

		// NOTHING WAS FOUND, REPORT -2 PATH AVAILIABLE
		if (error == ERROR_FILE_NOT_FOUND) {
			
			return(-2);

		// CHECK THE NETWORK PATH BY CREATING A FILE THERE
		} else if (error == ERROR_BAD_NETPATH) {

			// COMPOSE A PATH TO A FILE IN THE FOLDER
			string check;
			check = TextGuid();
			check = make(path, "\\program ", check, ".db");

			// CREATE THE FILE
			HANDLE file;
			file = FileWriteOpen(check, 2); // HIDDEN AND SYSTEM

			// THE FILE WAS CREATED, CLOSE AND DELETE IT AND REPORT -1 PATH IN USE BY FOLDER
			if (file) {

				FileWriteClose(file);
				FileDelete(check);
				return(-1);

			// THE FILE COULD NOT BE CREATED, REPORT -3 ERROR
			} else {

				return(-3);
			}

		// ERROR
		} else {
			
			return(-3);
		}
	}

	// CLOSE THE SEARCH
	FindClose(findfile);

	// GET ATTRIBUTES TO DETERMINE IF PATH IS A FILE OR FOLDER
	DWORD attributes;
	attributes = GetFileAttributes(s);

	// ERROR
	if (attributes == -1) {

		return(-3);

	// REPORT -1 PATH IN USE BY FOLDER
	} else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {

		return(-1);

	// GET THE DATE AND REPORT 0+ PATH IN USE BY FILE OF THAT SIZE
	} else {

		if (date) *date = info.ftLastWriteTime;
		return((int)info.nFileSizeLow);
	}
}

string TextStrip(read r)
{
	// takes text
	// removes all characters except for 0-9 a-z - _
	// returns a string

	// LOOP DOWN EACH CHARACTER IN THE TEXT
	int n, i;
	n = length(r);
	string s;
	for (i = 0; i < n; i++) {

		// THE CHARACTER IS ONE TO KEEP
		if ((r[i] >= '0' && r[i] <= '9') ||
			(r[i] >= 'a' && r[i] <= 'z') ||
			r[i] == '-' ||
			r[i] == '_') {

			// COPY IT ONTO THE STRING
			s += clip(r, i, 1);
		}
	}

	// RETURN THE STRING
	return(s);
}

string TextTime(DWORD time)
{
	// takes a number of milliseconds
	// composes text to describe how long that is
	// returns a string

	// RETURN EXPLINATION FOR LESS THAN A SECOND
	if (time < 1000) return(make("Done in less than a second"));

	// CALCULATE THE HOUR, MINUTE, AND SECOND NUMBERS
	int hour, minute, second;
	hour = time / 3600000;
	minute = (time / 60000) - (hour * 60);
	second = (time / 1000) - (hour * 3600) - (minute * 60);

	// COMPOSE THE TEXT TO DISPLAY AND RETURN IT
	string s;
	s = "Done in";
	if (hour) s += " " + numerals(hour) + " hr";
	if (hour || minute) s += " " + numerals(minute) + " min";
	s += " " + numerals(second) + " sec";
	return(s);
}
