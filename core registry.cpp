
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

bool RegistryReadNumber(HKEY root, read path, read name, int *i)
{
	// takes a root key handle, a key path, a registry variable name, and access to an integer to write the value
	// gets the information from the registry
	// writes under i and returns true if successful, or writes 0 and returns false if any error

	// OPEN THE KEY
	HKEY key;
	key = RegistryOpen(root, path);
	if (!key) return(false);

	// READ THE NUMBER VALUE
	DWORD size;
	size = sizeof(DWORD);
	int result;
	result = RegQueryValueEx(
		key,       // HANDLE TO AN OPEN KEY
		name,      // NAME OF THE VALUE TO READ
		0,
		NULL,
		(LPBYTE)i, // DATA BUFFER
		&size);    // SIZE OF DATA BUFFER
	if (result != ERROR_SUCCESS) {

		if (result != ERROR_FILE_NOT_FOUND) Report("registryreadnumber: error regqueryvalueex other than not found");
		RegistryClose(key);
		*i = 0;
		return(false);
	}

	// CLOSE THE KEY AND REPORT SUCCESS
	RegistryClose(key);
	return(true);
}

bool RegistryReadText(HKEY root, read path, read name, string *s)
{
	// takes a root key handle, a key path, a registry variable name, and access to a string to write the value
	// gets the information from the registry
	// writes under s and returns true if successful, or blanks and returns false if any error

	// OPEN THE KEY
	HKEY key;
	key = RegistryOpen(root, path);
	if (!key) return(false);

	// GET THE SIZE REQUIRED
	DWORD size;
	int result;
	result = RegQueryValueEx(
		key,    // HANDLE TO AN OPEN KEY
		name,   // NAME OF THE VALUE TO READ
		0,
		NULL,
		NULL,   // NO DATA BUFFER TO GET THE SIZE
		&size); // WRITE THE REQUIRED SIZE HERE
	if (result != ERROR_SUCCESS) {

		if (result != ERROR_FILE_NOT_FOUND) Report("registryreadtext: error regqueryvalueex getting size other than not found");
		RegistryClose(key);
		*s = "";
		return(false);
	}

	// OPEN A STRING
	string s2;
	write buffer = s2.GetBuffer(size - 1);
	buffer[size - 1] = '\0';

	// READ THE BINARY DATA
	result = RegQueryValueEx(
		key,            // HANDLE TO AN OPEN KEY
		name,           // NAME OF THE VALUE TO READ
		0,
		NULL,
		(LPBYTE)buffer, // DATA BUFFER
		&size);         // SIZE OF DATA BUFFER
	s2.ReleaseBuffer(); // CLOSE THE STRING
	if (result != ERROR_SUCCESS) {

		Report("registryreadtext: error regqueryvalueex getting text");
		RegistryClose(key);
		*s = "";
		return(false);
	}

	// CLOSE THE KEY, COPY THE STRING, AND REPORT SUCCESS
	RegistryClose(key);
	*s = s2;
	return(true);
}

bool RegistryWriteNumber(HKEY root, read path, read name, int i)
{
	// takes a root key handle, a key path, a registry variable name, and an integer
	// stores the information in the registry
	// returns true if successful, false if any error

	// OPEN THE KEY
	HKEY key;
	key = RegistryOpen(root, path);
	if (!key) return(false);

	// SET OR MAKE AND SET THE NUMBER VALUE
	int result;
	result = RegSetValueEx(
		key,              // HANDLE TO AN OPEN KEY
		name,             // NAME OF THE VALUE TO SET OR MAKE AND SET
		0,
		REG_DWORD,        // VARIALBE TYPE IS A 32 BIT NUMBER
		(const BYTE *)&i, // ADDRESS OF THE VALUE DATA TO LOAD
		sizeof(DWORD));   // SIZE OF THE VALUE DATA
	if (result != ERROR_SUCCESS) {

		Report("registrywritenumber: error regsetvalueex");
		RegistryClose(key);
		return(false);
	}

	// CLOSE THE KEY AND REPORT SUCCESS
	RegistryClose(key);
	return(true);
}

bool RegistryWriteText(HKEY root, read path, read name, read r)
{
	// takes a root key handle, a key path, a registry variable name, and value text
	// stores the information in the registry
	// returns true if successful, false if any error

	// OPEN THE KEY
	HKEY key;
	key = RegistryOpen(root, path);
	if (!key) return(false);

	// SET OR MAKE AND SET THE TEXT VALUE
	int result;
	result = RegSetValueEx(
		key,                // HANDLE TO AN OPEN KEY
		name,               // NAME OF THE VALUE TO SET OR MAKE AND SET
		0,
		REG_SZ,             // VARIALBE TYPE IS A NULL TERMINATED STRING
		(const BYTE *)r,    // ADDRESS OF THE VALUE DATA TO LOAD
		length(r) + 1); // SIZE OF THE VALUE DATA
	if (result != ERROR_SUCCESS) {

		Report("registrywritetext: error regsetvalueex");
		RegistryClose(key);
		return(false);
	}

	// CLOSE THE KEY AND REPORT SUCCESS
	RegistryClose(key);
	return(true);
}

bool RegistryDelete(HKEY root, read path, read name)
{
	// takes a root key handle, a key path beyond which exist no subkeys, and a registry variable name or blank to delete the key
	// deletes the registry variable or key from the registry
	// returns true if successful, false if any error

	// DELETE A REGISTRY KEY VARIABLE
	int result;
	if (is(name)) {

		// OPEN THE KEY
		HKEY key;
		key = RegistryOpen(root, path);
		if (!key) return(false);

		// DELETE THE VARIABLE
		result = RegDeleteValue(key, name);
		if (result != ERROR_SUCCESS) { Report("registrydelete: error regdeletevalue"); RegistryClose(key); return(false); }

		// CLOSE THE KEY AND REPORT SUCCESS
		RegistryClose(key);
		return(true);

	// DELETE A REGISTRY KEY
	} else {

		// DELETE THE KEY
		result = RegDeleteKey(root, path);
		if (result != ERROR_SUCCESS) { Report("registrydelete: error regdeletekey"); return(false); }

		// REPORT SUCCESS
		return(true);
	}
}

HKEY RegistryOpen(HKEY root, read path)
{
	// takes a root key handle and a key path
	// opens or creates and opens the key with full access
	// returns a handle to the open key, or null if any error

	// OPEN OR CREATE AND OPEN THE KEY
	HKEY key;
	DWORD info;
	int result;
	result = RegCreateKeyEx(
		root,                    // HANDLE TO OPEN ROOT KEY
		path,                    // SUBKEY NAME
		0,
		"",
		REG_OPTION_NON_VOLATILE, // SAVE INFORMATION IN REGISTRY FILE
		KEY_ALL_ACCESS,          // GET FULL ACCESS TO THE KEY
		NULL,
		&key,                    // THE OPENED OR CREATED KEY HANDLE IS PUT HERE
		&info);                  // TELLS IF THE KEY WAS OPENED OR CREATED AND OPENED
	if (result != ERROR_SUCCESS) { Report("registryopen: error regcreatekeyex"); return(NULL); }

	// RETURN THE OPEN KEY
	return(key);
}

bool RegistryClose(HKEY key)
{
	// takes an open key handle
	// closes the key
	// returns true if successful, false if any error

	// CLOSE THE KEY
	int result;
	result = RegCloseKey(key); // HANDLE TO AN OPEN KEY
	if (result != ERROR_SUCCESS) { Report("registryclose: error regclosekey"); return(false); }

	// REPORT SUCCESS
	return(true);
}
