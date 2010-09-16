
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

// Called when the clipboard contents change
// For each group of lines that begin "PAGE:", calls Page() on them
void ClipboardChanged() {

	string s;
	if (!ClipboardPaste(&s)) return; // Get the text on the clipboard

	std::vector<string> lines = Lines(s, true); // Include blank lines

	std::vector<string> page;
	boolean found = false;
	for (int i = 0; i < (int)lines.size(); i++) {

		if (starts(lines[i], "PAGE:")) { // Found a page

			found = true;
			if (page.size() > 0) Page(page); // Print the previous page
			page.clear();
		}

		if (found) page.push_back(lines[i]); // Add the line to the current page
	}

	if (found && page.size() > 0) Page(page); // Print the last page
}

// Given lines that make up a page, print that page to disk
void Page(std::vector<string> lines) {

	lines = Group(lines); // Look for the hyphen line to group neighboring lines together

	if (lines.size() < 1) return; // Only print a page that has content

	string titletext, templatepath, folderpath, savepath; // Read the template and title from the first line
	Path(lines[0], &titletext, &templatepath, &folderpath, &savepath);
	lines.erase(lines.begin());

	string templatecontents = FileOpen(templatepath); // Read in the template
	if (!has(templatecontents, "TITLE")) return;

	string body = Format(lines); // Convert wikitext to HTML

	string page = templatecontents; // Fill out the template
	page = replace(page, "TITLE", titletext);
	page = replace(page, "BODY", body);

	CreateDirectory(folderpath, NULL); // Save the page to disk
	FileSave(savepath, page);
}

// Group lines together using hyphen alone on a line to switch between grouping modes
std::vector<string> Group(std::vector<string> raw) {

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

	if (is(paragraph)) processed.push_back(paragraph); // Include last paragraph
	return processed;
}

// Given template text like "PAGE:Template Name:Title Text"
// Running in "C:\Documents\winwik.exe", composes text and paths like this
//    titletext like "Title Text"
// templatepath like "C:\Documents\Template-Name.html"            The path to the HTML template to open
//   folderpath like "C:\Documents\Template-Name"                 The folder to make
//     savepath like "C:\Documents\Template-Name\Title-Text.html" The file to save
void Path(read r, string *titletext, string *templatepath, string *folderpath, string *savepath) {

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

// Format wikitext into HTML
string Format(std::vector<string> lines) {

	string s;
	for (int i = 0; i < (int)lines.size(); i++) { // Loop for each line we were given
		s = lines[i];

		s = Heading(s); // Horizontal rules and headings
		s = Link(s); // Create links
		s = Curl(s); // Curl quotes and apostrophes
		s = replace(s, "--", "&mdash;"); // Em dash
		s = replace(s, "...", "&hellip;"); // Ellipsis
		s = Paragraph(s); // Paragraph tags

		lines[i] = s; // Put it back in the vector
	}

	s = Combine(lines); // Combine everything into a single string to format tags that might span lines
	s = Pair(s, "*", "*", "<b>", "</b>"); // Bold
	s = Pair(s, "_", "_", "<i>", "</i>"); // Italic
	s = Pair(s, "{", "}", "<span class=\"highlight\">", "</span>"); // Highlight

	lines = Lines(s, false); // Split it back into lines, removing blank ones this time
	s = Wrap(lines); // Wrap the lines of HTML to a pleasing column width
	return s;
}

// Convert "My Title" to "My-Title"
string Hyphen(read r) {

	return replace(r, " ", "-");
}

// Given a line of wikitext, convert it into a horizontal rule or heading if it is one
string Heading(read r) {

	string s = r;
	if (s == "----") s = "<hr>"; // Horizontal rule
	if      (starts(s, "====")) { s = off(s, "===="); s = "<h4>" + s + "</h4>"; } // Heading
	else if (starts(s, "==="))  { s = off(s, "===");  s = "<h3>" + s + "</h3>"; }
	else if (starts(s, "=="))   { s = off(s, "==");   s = "<h2>" + s + "</h2>"; }
	else if (starts(s, "="))    { s = off(s, "=");    s = "<h1>" + s + "</h1>"; }
	return s;
}

// Turn a line of wikitext into a HTML paragraph if it's not already a heading or horizontal rule
string Paragraph(read r) {

	string s = r;
	if (!starts(s, "<h")) s = "<p>" + s + "</p>"; // Headings and horizontal rules both start "<h"
	return s;
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

// Curl quotes and apostrophes
string Curl(read r) {

	string s = make(" ", r, " "); // Put spaces on ends to curl starting and ending quotes
	s = replace(s, "'", "&rsquo;"); // Curl apostrophes
	s = replace(s, " \"", " &ldquo;");
	s = replace(s, "\" ", "&rdquo; ");
	return trim(s, " ");
}

// Turn wikitext links into HTML
string Link(read r) {

	string raw = r;
	string processed;
	string a, b, c;

	while (is(raw)) { // Split raw into "aaaa[bbbb]cccc"

		if (!has(raw, "[")) break;
		split(raw, "[", &a, &b);

		if (!has(b, "]")) break;
		split(b, "]", &b, &c);

		processed += a + Target(b); // Create the link we found
		raw = c;
	}

	processed += raw;
	return processed;
}

// Given the part of link wikitext inside the braces, compose the HTML of the link
string Target(read r) {

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

	return "<a href=\"" + target + "\">" + anchor + "</a>"; // Compose HTML
}

// Given a list of lines, wrap long ones onto multiple lines and combine them all into a single string
string Wrap(std::vector<string> lines) {

	string s, line;

	for (int i = 0; i < (int)lines.size(); i++)
		s += Break(lines[i]) + "\r\n\r\n"; // Wrap each line and include a blank line after it

	return trim(s, "\r\n"); // Don't put a newline at the very end of the whole thing
}

// Break a long line into several lines to wrap the text nicely
string Break(read r) {

	std::vector<string> words = Words(r); // Break the text into a list of words

	string lines, line, word;
	for (int i = 0; i < (int)words.size(); i++) { // Loop for each word
		word = words[i];

		if (length(word) > WRAP_WIDTH) { // By itself

			lines += trim(line, " ") + "\r\n" + word + "\r\n"; // Don't put a space at the end of a line
			line = "";

		} else if (length(line) + length(word) > WRAP_WIDTH) { // On the next line

			lines += trim(line, " ") + "\r\n";
			line = word + " ";

		} else { // On the current line

			line += word + " ";
		}
	}

	lines += trim(line, " ") + "\r\n";
	return trim(lines, "\r\n"); // End the whole thing without a newline
}

// Break text into a list of words
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

// Split a string into a list of trimmed lines, true to include blank lines
std::vector<string> Lines(read r, bool blank) {

	string s = r;
	string line;
	std::vector<string> lines;

	while (is(s)) {

		split(s, "\n", &line, &s);
		line = trim(line, " ", "\r", "\t");
		if (blank || is(line)) lines.push_back(line); // Only include line if we're allowing blanks or it has text
	}
	return lines;
}

// Combine a list of lines into a single string by putting newlines between them
string Combine(std::vector<string> lines) {

	string s;
	for (int i = 0; i < (int)lines.size(); i++)
		s += lines[i] + "\r\n";
	return trim(s, "\r\n"); // No newline at the end
}
