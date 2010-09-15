
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




// Print a list of lines to the debugging log
void Print(std::vector<string> v) {

	for (int i = 0; i < (int)v.size(); i++)
		log(v[i]);
}

// Splits a string into a list of trimmed lines, blank lines included
std::vector<string> Lines(read r) {

	string s = r;
	string line;
	std::vector<string> lines;

	while (is(s)) {

		split(s, "\n", &line, &s);
		line = trim(line, " ", "\r", "\t");
		lines.push_back(line);
	}
	return lines;
}

// Combine a list of lines into a single string
string Combine(std::vector<string> v) {

	string s;
	for (int i = 0; i < (int)v.size(); i++)
		s += v[i] + "\r\n";
	return s;
}

// Convert "My Title" to "My-Title"
string SpacesToHyphens(read r) {

	return replace(r, " ", "-");
}

// Given template text like "PAGE:Template Name:Title Text"
// Running in "C:\Documents\winwik.exe", composes text and paths like this
//    titletext like "Title Text"
// templatepath like "C:\Documents\Template-Name.html"            The path to the HTML template to open
//   folderpath like "C:\Documents\Template-Name"                 The folder to make
//     savepath like "C:\Documents\Template-Name\Title-Text.html" The file to save
void DetermineLocation(read r, string *titletext, string *templatepath, string *folderpath, string *savepath) {

	string both = off(r, "PAGE:");
	string templatetext;
	split(both, ":", &templatetext, titletext);

	character bay[MAX_PATH];
	GetModuleFileName(NULL, bay, MAX_PATH);
	string folder = before(bay, "\\", Reverse) + "\\";

	*templatepath = make(folder, SpacesToHyphens(templatetext), ".html");
	*folderpath   = make(folder, SpacesToHyphens(templatetext));
	*savepath     = make(folder, SpacesToHyphens(templatetext), "\\", SpacesToHyphens(*titletext), ".html");
}


// r like "once before {things were} different"
// tag1 and tag2 like "{" and "}"
// before, inside, and after like "once before " "things were" " different"

bool SplitQuoted(read r, read tag1, read tag2, string *before, string *inside, string *after) {

	// find the first { that is at the very start or has a space before it



	return false;

}


bool SplitStarting(read r, read t, string *before, string *after) {

	string s = r;

	while (true) {

		int f = find(s, t);
		if (f == -1) return false;
		if (f == 0) {

			split(r, t, before, after);
			return true;
		}

		character b = r[f - 1];





	}

}


// The index in r of the first appearance of t that has a space before it, or is at the very start, or -1 not found
int FindStarting(read r, read t) {

	string s = r;
	int i = 0;

	while (true) {

		int f = find(s, t);
		if (f == -1 || f == 0 || f == length(r) - length(t)) return i + f; // Not found, start, or end

		character b = r[i - 1]



	}






}

// The index in r of the first appearance of t that has a space after it, or is at the very end, or -1 not found
int FindEnding(read r, read t) {
	

	return -1;
}




// Convert r "*text*" to "<b>text</b>"
// Using      w1   w2     h1     h2 wikitext and html tags
string Tag(read r, read w1, read w2, read h1, read h2) {

	string raw = r;
	string processed;
	string a, b, c;

	while (is(raw)) {

		if (!has(raw, w1)) break;
		split(raw, w1, &a, &b);

		if (!has(b, w2)) break;
		split(b, w2, &b, &c);

		processed += make(a, h1, b, h2);
		raw = c;
	}

	processed += raw;
	return processed;
}


// Curl quotes and apostrophes
string Curl(read r) {

	return "";
}

// Convert "..." to the ellipsis character
string Ellipsis(read r) {

	return "";
}

// Convert "--" to the em dash
string Dash(read r) {

	return "";
}








string Format(std::vector<string> lines) {

	string s;
	s = Combine(lines);

	return s;
}



void Page(std::vector<string> lines) {

	if (lines.size() < 1) return;

	string titletext, templatepath, folderpath, savepath;
	DetermineLocation(lines[0], &titletext, &templatepath, &folderpath, &savepath);
	lines.erase(lines.begin());

	/*
	log("titletext:    ", titletext);
	log("templatepath: ", templatepath);
	log("folderpath:   ", folderpath);
	log("savepath:     ", savepath);
	*/

	string templatecontents = FileOpen(templatepath);
	if (!has(templatecontents, "TITLE")) return;


	string body = Format(lines);


	string page = templatecontents;
	page = replace(page, "TITLE", titletext);
	page = replace(page, "BODY", body);

	CreateDirectory(folderpath, NULL);
	FileSave(savepath, page);


}

// Called when the clipboard contents change
// For each group of lines that begin "PAGE:", calls Page() on them
void ClipboardChanged() {

	string s;
	if (!ClipboardPaste(&s)) return;

	std::vector<string> lines = Lines(s);

	std::vector<string> page;
	boolean found = false;
	for (int i = 0; i < (int)lines.size(); i++) {

		if (starts(lines[i], "PAGE:")) {

			found = true;
			if (page.size() > 0) Page(page);
			page.clear();
		}

		if (found) page.push_back(lines[i]);
	}

	if (found && page.size() > 0) Page(page);
}




void TestWiki() {

	log(numerals(FindStarting("at the end tag", "tag")));

}


