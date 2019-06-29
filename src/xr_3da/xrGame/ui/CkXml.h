// CkXml.h: interface for the CkXml class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKXML_H
#define _CKXML_H

#pragma once

class CkByteData;
class CkCert;
class CkString;

class CkXml  
{
    private:
	void *m_impl;

	// Don't allow assignment or copying these objects.
	CkXml(const CkXml &) { } 
	CkXml &operator=(const CkXml &) { return *this; }

    public:
	void *getImpl(void) { return m_impl; } 

	CkXml(void *impl) : m_impl(impl) { }

	CkXml();
	virtual ~CkXml();

        CkXml *HttpPost(const char *url);

	bool SetBinaryContent(const unsigned char *data, unsigned long dataLen, 
	    bool zipFlag, bool encryptFlag, const char *password);

	bool GetBinaryContent(CkByteData &data, 
	    bool unzipFlag, bool decryptFlag, const char *password);

	bool ZipTree();
	bool ZipContent();
	bool UnzipTree();
	bool UnzipContent();

	bool EncryptContent(const char *password);
	bool DecryptContent(const char *password);
	//bool SignContent(CkCert *cert);
	//bool VerifyContent(CkCert *cert);

	//bool ConvertToEncoding(const char *encoding);
	CkXml *GetRoot(void);
	CkXml *GetChildWithTag(const char *tag);
	long get_TreeId(void);
	CkXml *PreviousSibling(void);
	CkXml *NextSibling(void);
	bool PreviousSibling2(void);
	bool NextSibling2(void);
	CkXml *LastChild(void);
	CkXml *FirstChild(void);
	bool Clear();
	void Copy(CkXml *node);
	CkXml *GetParent(void);

	CkXml *SearchForAttribute(CkXml *after, const char *tag, const char *attr, const char *valuePattern);
	CkXml *SearchAllForContent(CkXml *after, const char *contentPattern);
	CkXml *SearchForContent(CkXml *after, const char *tag, const char *contentPattern);
	CkXml *SearchForTag(CkXml *after, const char *tag);

	bool SearchForAttribute2(CkXml *after, const char *tag, const char *attr, const char *valuePattern);
	bool SearchAllForContent2(CkXml *after, const char *contentPattern);
	bool SearchForContent2(CkXml *after, const char *tag, const char *contentPattern);
	bool SearchForTag2(CkXml *after, const char *tag);
	bool GetNthChildWithTag2(const char *tag, long n);

	CkXml *FindChild(const char *tag);
	CkXml *FindOrAddNewChild(const char *tag);
	CkXml *NewChild(const char *tag, const char *content);
	void NewChild2(const char *tag, const char *content);
	CkXml *GetNthChildWithTag(const char *tag, long n);
	long NumChildrenHavingTag(const char *tag);
	CkXml *ExtractChildByName(const char *tag, const char *attrName, const char *attrValue);
	CkXml *ExtractChildByIndex(long index);
	void RemoveFromTree(void);
	CkXml *GetChild(long index);
	bool AddChildTree(CkXml *tree);
	bool SwapTree(CkXml *tree);
	bool SwapNode(CkXml *node);
	bool HasAttrWithValue(const char *name, const char *value);
	bool GetAttrValue(const char *name, CkString &str);
	bool GetAttributeValue(long index, CkString &str);
	bool GetAttributeName(long index, CkString &str);
	bool RemoveAllAttributes();
	bool RemoveAttribute(const char *name);
	bool AddAttribute(const char *name, const char *value);
	bool AppendToContent(const char *str);
	bool GetXml(CkString &str);
	bool SaveXml(const char *fileName);
	bool LoadXmlFile(const char *fileName);
	bool LoadXml(const char *xmlData);

	const char *get_Version(void) const;
	bool get_Version(CkString &str);

	bool get_Cdata();
	bool put_Cdata(bool newVal);

	//bool get_ValidatingParser();
	//bool put_ValidatingParser(bool newVal);

	long get_NumChildren();

	const char *get_Content(void) const;
	bool get_Content(CkString &str);
	bool put_Content(const char *newVal);

	bool GetChildContent(const char *tag, CkString &str);

	const char *get_Tag(void) const;
	bool get_Tag(CkString &str);
	bool put_Tag(const char *newVal);

	long get_NumAttributes();

	bool get_Standalone(CkString &str);  // yes or no
	bool put_Standalone(const char *newVal);

	bool get_Encoding(CkString &str);  // such as iso-8859-1
	bool put_Encoding(const char *newVal);

	bool get_HttpParamName(CkString &str);
	bool put_HttpParamName(const char *newVal);

	//bool get_AutoFix();
	//bool put_AutoFix(bool newVal);

	bool get_SingleThreadOpt();
	bool put_SingleThreadOpt(bool newVal);

	// Error log retrieval and saving (these methods are common to all Chilkat VC++ classes.)
	bool SaveXmlLog(const char *filename);
        void GetLogXml(CkString &str);
        void GetLogHtml(CkString &str);
        void GetLogText(CkString &str);

	bool QEncodeContent(const char *charset, const CkByteData &db);
	bool BEncodeContent(const char *charset, const CkByteData &db);
	bool DecodeContent(CkByteData &db);

	bool UpdateAttribute(const char *attrName, const char *attrValue);


	void SortByTag(long ascending);
	void SortByContent(long ascending);
	void SortByAttribute(const char *attrName, long ascending);

	void SortRecordsByContent(const char *sortTag, long ascending);
	void SortRecordsByAttribute(const char *sortTag, const char *attrName, long ascending);
	CkXml *FindNextRecord(const char *tag, const char *contentPattern);

};


#endif


