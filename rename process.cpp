
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

void RenameList(read folder)
{
	// takes a folder path with a trailing slash
	// calls rename prepare on each file path in the tree of folders
	// returns nothing

	// LOOK IN THE FOLDER AND LOOP TO GET THE PATH OF EACH FOLDER AND FILE THERE
	HANDLE find;
	find = NULL;
	string path;
	while (FileList(folder, "*.*", &find, &path)) {

		if (trails(path, "\\")) RenameList(path);    // FOLDER, MOVE INTO THE FOLDER WITH THIS FUNCTION
		else                    RenamePrepare(path); // FILE
	}
}

void RenamePrepare(read path)
{
	// takes a file path
	// if it should be renamed, moves it into the done folder
	// returns nothing

	// LOOK AT THE FILE
	FILETIME date;
	int look;
	look = RenameFileLook(path, &date);

	// DETERINE IF THE FILE IS AN IMAGE
	string s;
	s = after(path, "\\", Reverse, Different);
	bool image;
	image = false;
	if ((trails(s, ".bmp",  Matching) ||
		 trails(s, ".png",  Matching) ||
		 trails(s, ".gif",  Matching) ||
		 trails(s, ".jpg",  Matching) ||
		 trails(s, ".jpe",  Matching) ||
		 trails(s, ".jpeg", Matching)) && !starts(s, "tn_", Matching)) image = true;

	// ERROR
	if (look < 0) {

		// DOCUMENT THE ERROR
		RenameDisplayError(make("Looking at •", path, "•"));

	// SKIP
	} else if (look == 0 || (State.rename.onlyimages && !image)) {

		// COUNT THE SKIPPED FILE
		State.rename.skipped++;
		RenameDisplayStatus();

	// PREP
	} else {

		// DISPLAY THE TASK
		RenameDisplayTask(make("Preparing ", path));

		// ALLOCATE A NEW RENAME ITEM
		renameitem *r = new renameitem;

		// SET ID
		r->id = State.rename.id;
		State.rename.id++;

		// COMPOSE THE NEW NAME AND EXTENSION FOR THE FILE
		ComposeName(path, &r->name, &r->ext);

		// SET SIZE AND DATE
		r->size = look;
		r->date = date;

		// INITIALIZE NUMBER AND FOLDER
		r->number = 0;
		r->folder = 0;

		// COMPOSE THE DESTINATION PATH FOR THE FILE
		string destination;
		destination = ComposePathPrepare(r);

		// FOLDER, MOVE AND RENAME THE FILE
		if (!FileMoveSimple(path, destination, 1, 0)) {

			// FREE RESOURCES, DOCUMENT THE ERROR, AND LEAVE
			RenameDisplayError(make("Moving •", path, "• to •", destination, "•"));
			delete r;
			return;
		}

		// CLEAR ALL FILE ATTRIBUTES
		if (!FileSet(destination, 0)) {

			// FREE RESOURCES, DOCUMENT THE ERROR, AND LEAVE
			RenameDisplayError("Clearing the attributes of •" + destination + "•");
			delete r;
			return;
		}

		// INSERT THE ITEM INTO THE LIST
		RenameInsert(r);

		// COUNT THE PREPARED FILE
		State.rename.prepared++;
		RenameDisplayStatus();
	}
}

void RenameDuplicateNameAndData()
{
	// takes nothing
	// marks duplicates, changing 0s in the list to -1 duplicate
	// returns nothing

	// PROGRESS COUNT
	int progress;
	progress = 0;

	// SCAN I AND J DOWN EACH PAIR OF ITEMS SKIPPING DUPLICATES AND NUMBERED FILES
	renameitem *i, *j;
	int level;
	level = 2;
	i = RenameNext(RenameFirst(level), level); // LEVEL 2 NAME SIZE
	while (i) {

		// POINT J TO EACH ITEM AFTER I WITH THE SAME NAME AND SIZE
		j = RenameNext(i->next[level], level);
		while (j && i->name == j->name && i->size == j->size) {

			// IF I AND J HAVE THE SAME NAME AND DATA, MARK J AS DUPLICATE
			if (RenameSameData(i, j)) j->number = -1;

			j = RenameNext(j->next[level], level);
		}

		// DISPLAY TASK PROGRESS
		progress++;
		RenameDisplayTask("Marked newer identicle name and data duplicates of " + saynumber(progress, "file"));

		i = RenameNext(i->next[level], level);
	}
}

void RenameDuplicateData()
{
	// takes nothing
	// marks duplicates, changing 0s in the list to -1 duplicate
	// returns nothing

	// PROGRESS COUNT
	int progress;
	progress = 0;

	// SCAN I AND J DOWN EACH PAIR OF ITEMS SKIPPING DUPLICATES AND NUMBERED FILES
	renameitem *i, *j;
	int level;
	level = 1;
	i = RenameNext(RenameFirst(level), level); // LEVEL 1 SIZE
	while (i) {

		// POINT J TO EACH ITEM AFTER I WITH THE SAME SIZE
		j = RenameNext(i->next[level], level);
		while (j && i->size == j->size) {

			// IF I AND J HAVE THE SAME DATA, MARK J AS DUPLICATE
			if (RenameSameData(i, j)) j->number = -1;

			j = RenameNext(j->next[level], level);
		}

		// DISPLAY TASK PROGRESS
		progress++;
		RenameDisplayTask("Marked newer identicle data duplicates of " + saynumber(progress, "file"));

		i = RenameNext(i->next[level], level);
	}
}

void RenameNumber()
{
	// takes nothing
	// numbers files, changing 0s in the list to 1+ number
	// returns nothing

	// DISPLAY TASK PROGRESS
	RenameDisplayTask("Numbering");

	// SCAN I AND J DOWN EACH PAIR OF ITEMS SKIPPING DUPLICATES AND NUMBERED FILES
	renameitem *i, *j;
	int level;
	level = 3;
	i = RenameNext(RenameFirst(level), level); // LEVEL 3 NAME DATE
	int number;
	while (i) {

		// MAKE I NUMBER 1 AND STORE THE NEXT NUMBER 2
		i->number = 1;
		number = 2;

		// POINT J TO EACH ITEM AFTER I WITH THE SAME NAME
		j = RenameNext(i->next[level], level);
		while (j && i->name == j->name) {

			// IF I AND J HAVE THE SAME NAME, SET THE NUMBER ON J AND INCRIMENT NUMBER
			j->number = number;
			number++;

			j = RenameNext(j->next[level], level);
		}

		i = j;
	}
}

void RenameGroup()
{
	// takes nothing
	// groups files into less than 90 mb folders
	// returns nothing

	// DISPLAY TASK PROGRESS
	RenameDisplayTask("Grouping");

	// VARIABLES FOR LOOPS
	renameitem *start, *a, *b, *c;
	start = a = b = c = RenameGroupNext(RenameFirst(0));
	int size, number;
	size = 0;
	number = 1;

	// LOOP POINTING EACH FILE IN A FOLDER AT THE NEWEST ONE THERE
	while (c) {

		// ADD THE SIZE TO THE TOTAL
		size += c->size;

		// THE TOTAL IS NOW TOO MUCH
		if (size > FOLDERSIZE) {

			// MOVE A FORWARD LINKING EACH MEMBER FILE TO THE NEWEST ONE AND SETTING THE FOLDER NUMBER
			while (a != c) {

				a->last = b;
				a->folder = number;
				a = RenameGroupNext(a->next[0]);
			}

			// RESET SIZE TO THE ONE THAT PUT IT OVER AND INCRIMENT THE FOLDER NUMBER
			size = c->size;
			number++;
		}

		// MOVE C FORWARD AND KEEP B RIGHT BEHIND IT
		b = c;
		c = RenameGroupNext(c->next[0]);
	}

	// C IS ON THE NULL BEYOND THE EDGE, LINK THE LAST FOLDER
	while (a) {

		a->last = b;
		a->folder = number;
		a = RenameGroupNext(a->next[0]);
	}
}

void RenameMove()
{
	// takes nothing
	// moves and renames files from the middle area to the dated folders
	// returns nothing

	// MAKE STRINGS
	string path, destination;

	// MOVE DOWN THE LIST
	renameitem *r;
	r = RenameFirst(0);
	bool result;
	while (r) {

		// SEPARATE
		if (r->number == -1) {

			// COUNT THE SEPARATED FILE
			State.rename.separated++;
			RenameDisplayStatus();

		// RENAME
		} else {

			// GET THE SOURCE, FOLDER, AND DESTINATION PATHS FOR THE FILE
			path = ComposePathPrepare(r);
			destination = ComposePathDestination(r);

			// DISPLAY THE TASK
			RenameDisplayTask("Renaming " + destination);

			// FOLDER, MOVE AND RENAME THE FILE
			result = FileMoveSimple(path, destination, 1, 0);
			if (!result) RenameDisplayError("Moving •" + path + "• to •" + destination + "•");

			// COUNT THE RENAMED FILE
			State.rename.renamed++;
			RenameDisplayStatus();
		}

		r = r->next[0];
	}
}
