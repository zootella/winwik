
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

void print(std::vector<string> v) {
	for (int i = 0; i < (int)v.size(); i++)
		log(v[i]);
}

// Splits a string into a list of lines
std::vector<string> lines(read r) {

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

void PrintStory(std::vector<string> lines) {

	log("story!:");
	print(lines);
}

void ParseGroupAndTitle(read r, string *group, string *title) {

	string s = off(r, "PAGE:");
	split(s, ":", group, title);
}

// Given template text like "PAGE:Template Name:Title Text"
// Composes three paths 
// running in  "C:\Documents\winwik.exe"
// group like  "C:\Documents\Template-Name.html"            The path to the HTML template to open
// folder like "C:\Documents\Template-Name"                 The folder to make
// file like   "C:\Documents\Template-Name\Title-Text.html" The file to save
void DetermineLocation(read r, string *group, string *folder, string *file) {

	string s = off(r, "PAGE:");
	string rawgroup, rawtitle;
	split(s, ":", rawgroup, rawtitle);

	string running = "";






}




void ClipboardChanged() {

	string s;
	if (!ClipboardPaste(&s)) return;


	std::vector<string> v = lines(s);

	std::vector<string> story;


	boolean in = false;

	for (int i = 0; i < (int)v.size(); i++) {

		if (starts(v[i], "PAGE:")) {
			in = true;

			if (story.size() > 0)
				PrintStory(story);
			story.clear();
		}

		if (in)
			story.push_back(v[i]);
	}

	if (in && story.size() > 0)
		PrintStory(story);





	/*
	string path;
	path = "C:\\Documents\\test.txt";
	string t = FileOpen(path);

	FileSave("C:\\Documents\\clipboard.txt", s);
	*/

}


