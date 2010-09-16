
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

// Splits a string into a list of trimmed lines, true to include blank lines
std::vector<string> Lines(read r, bool blank) {

	string s = r;
	string line;
	std::vector<string> lines;

	while (is(s)) {

		split(s, "\n", &line, &s);
		line = trim(line, " ", "\r", "\t");
		if (blank || is(line)) lines.push_back(line);
	}
	return lines;
}

// Combine a list of lines into a single string by putting newlines between them
string Combine(std::vector<string> v) {

	string s;
	for (int i = 0; i < (int)v.size(); i++) {

		s += v[i];
		if (i < (int)v.size() - 1) s += "\r\n";
	}
	return s;
}

// Convert "My Title" to "My-Title"
string Hyphen(read r) {

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

	*templatepath = make(folder, Hyphen(templatetext), ".html");
	*folderpath   = make(folder, Hyphen(templatetext));
	*savepath     = make(folder, Hyphen(templatetext), "\\", Hyphen(*titletext), ".html");
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

		character b = r[i - 1];



	}

	return -1;






}

// The index in r of the first appearance of t that has a space after it, or is at the very end, or -1 not found
int FindEnding(read r, read t) {
	

	return -1;
}




// Convert r "*text*" to "<b>text</b>"
// Using      ^w1  ^w2    ^^^h1  ^^^^h2 wikitext and html tags
string Pair(read r, read w1, read w2, read h1, read h2) {

	string raw = r;
	string processed;
	string a, b, c;

	while (is(raw)) { // Split raw into "aaaa*bbbb*cccc"

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


// Curl quotes
string Curl(read r) {

	string s = make(" ", r, " ");
	s = replace(s, "'", "&rsquo;");
	s = replace(s, " \"", " &ldquo;");
	s = replace(s, "\" ", "&rdquo; ");
	return trim(s, " ");
}

string LinkInside(read r) {

	// takes text inside the square braces

	string anchor, target;
	string b, a;

	if (has(r, "(")) {

		split(r, "(", &b, &a); // Wikitext link like [My(Link)]
		a = trim(a, ")");
		anchor = b;
		target = b + " " + a;

	} else if (has(r, ">")) {

		split(r, ">", &b, &a); // Like [My Link>to here]
		anchor = b;
		target = a;

	} else {

		anchor = r; // Just [My Link]
		target = r;
	}


	if (!has(target, ":") && !has(target, ".")) // Don't change http addresses or image.png
		target = Hyphen(target) + ".html"; // Link [My Link] to My-Link.html

	if (starts(target, "^")) // Use ^ to go up a folder
		target = replace(target, "^", "../");

	return "<a href=\"" + target + "\">" + anchor + "</a>";
}

string Link(read r) {

	string raw = r;
	string processed;
	string a, b, c;

	while (is(raw)) { // Split raw into "aaaa[bbbb]cccc"

		if (!has(raw, "[")) break;
		split(raw, "[", &a, &b);

		if (!has(b, "]")) break;
		split(b, "]", &b, &c);

		processed += a + LinkInside(b);
		raw = c;
	}

	processed += raw;
	return processed;
}



string Heading(read r) {

	string s = r;
	if (s == "----") s = "<hr>";
	if      (starts(s, "====")) { s = off(s, "===="); s = "<h4>" + s + "</h4>"; }
	else if (starts(s, "==="))  { s = off(s, "===");  s = "<h3>" + s + "</h3>"; }
	else if (starts(s, "=="))   { s = off(s, "==");   s = "<h2>" + s + "</h2>"; }
	else if (starts(s, "="))    { s = off(s, "=");    s = "<h1>" + s + "</h1>"; }
	return s;
}



std::vector<string> Combiner(std::vector<string> raw) {

	bool combine = false;
	string paragraph;
	std::vector<string> processed;

	for (int i = 0; i < (int)raw.size(); i++) {
		string line = trim(raw[i], " ", "\t", "\r");
		if (line == "-") {
			if (combine) { // Hyphen turns outlining off

				combine = false;
				if (is(paragraph)) processed.push_back(paragraph);
				paragraph = "";

			} else { // Hyphen turns outlining on

				combine = true;
			}

		} else if (isblank(line)) {
			if (combine) { // Blank line marks the end of a paragraph of lines

				if (is(paragraph)) processed.push_back(paragraph);
				paragraph = "";
			}

		} else {
			if (combine) { // Line of text with combine on

				if (is(paragraph)) paragraph += make(" ", line);
				else               paragraph = line;

			} else { // Line of text with combine off

				processed.push_back(line);
			}
		}
	}

	if (is(paragraph)) processed.push_back(paragraph);
	return processed;
}

string Paragraph(read r) {

	string s = r;

	if (!starts(s, "<h")) s = "<p>" + s + "</p>";

	return s;
}

std::vector<string> Words(read r) {

	string s = r;
	s = replace(s, "\r", " "); // Convert newline and tab characters into spaces
	s = replace(s, "\n", " ");
	s = replace(s, "\t", " ");

	while (has(s, "  ")) // Collapse multiple spaces down to a single space
		s = replace(s, "  ", " ");

	s = trim(s, " "); // Remove leading and trailing spaces

	std::vector<string> words;
	string word;
	while (is(s)) {

		split(s, " ", &word, &s); // Not found puts all text in before
		words.push_back(word);
	}
	return words;
}


#define WRAP_WIDTH 110



string WrapLong(read r) {

	std::vector<string> words = Words(r);

	string lines, line, word;

	for (int i = 0; i < (int)words.size(); i++) {
		word = words[i];

		if (length(word) > WRAP_WIDTH) { // By itself

			lines += trim(line, " ") + "\r\n" + word + "\r\n";
			line = "";

		} else if (length(line) + length(word) > WRAP_WIDTH) { // On the next line

			lines += trim(line, " ") + "\r\n";
			line = word + " ";

		} else { // On the current line

			line += word + " ";
		}
	}

	lines += trim(line, " ") + "\r\n";
	return trim(lines, "\r\n");
}

string Wrap(std::vector<string> lines) {

	string s, line;

	for (int i = 0; i < (int)lines.size(); i++)
		s += WrapLong(lines[i]) + "\r\n\r\n";

	return trim(s, "\r\n");
}




string Format(std::vector<string> lines) {



	string s;
	for (int i = 0; i < (int)lines.size(); i++) {

		s = lines[i];

		s = Heading(s); // Horizontal rules and headings
		s = Link(s); // Create links
		s = Curl(s); // Curl quotes and apostrophes
		s = replace(s, "--", "&mdash;"); // Em dash
		s = replace(s, "...", "&hellip;"); // Ellipsis

		s = Paragraph(s);

		lines[i] = s;
	}


	s = Combine(lines);

	s = Pair(s, "*", "*", "<b>", "</b>"); // Bold
	s = Pair(s, "_", "_", "<i>", "</i>"); // Italic
	s = Pair(s, "{", "}", "<span class=\"highlight\">", "</span>"); // Highlight

	lines = Lines(s, false); // Remove blank lines

	s = Wrap(lines);



	return s;
}



void Page(std::vector<string> lines) {

	lines = Combiner(lines);

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

	std::vector<string> lines = Lines(s, true); // Include blank lines

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


