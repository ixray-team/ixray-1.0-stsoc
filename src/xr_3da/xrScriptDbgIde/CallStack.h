// CallStack.h: interface for the CCallStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLSTACK_H__1BC34BF3_8062_4E58_AAEC_8DA50AB351A2__INCLUDED_)
#define AFX_CALLSTACK_H__1BC34BF3_8062_4E58_AAEC_8DA50AB351A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScintillaView;
struct SCNotification;

#include "ScintillaBar.h"

class CCallStack : public CScintillaBar
{
public:
	void GotoStackTraceLevel(int nLevel);
	void Add(const char* szDesc, const char* szFile, int nLine);
	void Clear();
	CCallStack();
	virtual ~CCallStack();

	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT);
	virtual int OnSci(CScintillaView* pView, SCNotification* pNotify);
	int GetLevel() { return m_nCurrentLevel; };
protected:
	int m_nCurrentLevel;
	CUIntArray	m_levels, m_lines;
	CStringArray m_files;
};

#endif // !defined(AFX_CALLSTACK_H__1BC34BF3_8062_4E58_AAEC_8DA50AB351A2__INCLUDED_)
