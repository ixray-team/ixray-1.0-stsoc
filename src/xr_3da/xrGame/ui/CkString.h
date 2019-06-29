// CkString.h: interface for the CkString class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSTRING_H
#define _CKSTRING_H

#pragma once


// Chilkat provides a simple string class for convenience.
// We decided against using the MFC CString or STL string classes to
// prevent linkage requirements to these libraries.
// We wanted Chilkat to be usable from both MFC and non-MFC applications.
class CkString 
{
    private:
	void *m_impl;

    public:
	void *getImpl(void) { return m_impl; }

	CkString();
	~CkString();

	CkString(const CkString &);
	CkString &operator=(const CkString &);

	// Load the contents of a text file into the CkString object.
        bool loadFile(const char *fileName);

	char charAt(int idx) const;
        const char *pCharAt(int idx) const;

        int intValue(void);
        double doubleValue(void);

	void clear(void);
	void prepend(const char *s);
	void append(int n);
	void append(const char *s);
        void appendChar(char c);
	void appendN(const char *s, int n);	// Append N characters to the string.
        void append(const CkString &str);

	// Convert the binary data to a hex string representation and append.
        void appendHexData(const unsigned char *data, int dataLen);

	// Same as clearing the string and appending.
        void setString(const char *s);

	// Same as xr_strcmp
        int compare(const char *s) const;

	const char *getString(void) const;
	int getSize(void) const;

        // Return the string in newly allocated memory.  The CkString is empty after this.
	char *extractString(int *length);
        // Make a copy and return it in newly allocated memory.
	char *copyString(int *length);

        // Trim whitespace from both ends of the string.
        void trim(void);    // Does not include newline
        void trim2(void);   // Includes newline

        // Case sensitive replacement functions.
	// Return the number of occurances replaced.
        int replaceAllOccurances(const char *pattern, const char *replacement);
        bool replaceFirstOccurance(const char *pattern, const char *replacement);

        // CRLF
        void toCRLF(void);                  // Make all end of lines a CRLF ("\r\n")
        void toLF(void);                    // Make all end of lines a "\n"

        // Eliminate all occurances of a particular character.
        void eliminateChar(char c, int startIndex);

        // Return the last character in the string.
        char lastChar(void) const;
        // Remove the last N chars from the string.
        void shorten(int n);    

        // Convert to lower or upper case
        void toLowerCase(void);
        void toUpperCase(void);

        // Convert XML special characters to their representations
        // Example: '<' is converted to &lt;
        void encodeXMLSpecial(void);    // Convert '<' to &lt;
        void decodeXMLSpecial(void);    // Convert &lt; to '<'

	// Pattern can contain * and ? wildcards.
        bool containsSubstring(const char *pattern);
        bool containsSubstringNoCase(const char *pattern);

        void appendLastWindowsError(void);

        bool equals(const char *s) const;
        bool equals(const CkString &s) const;
        bool equalsIgnoreCase(const char *s) const;
        bool equalsIgnoreCase(const CkString &s) const;
        
        // Remove a chunk from the string.
        void removeChunk(int idx, int length);

        // Remove all occurances of a particular character.
        void removeCharOccurances(char c);

};

#endif
