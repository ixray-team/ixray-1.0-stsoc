#if !defined(AFX_MDICLIENTWND_H__A19DDD02_2C26_4675_A107_262E4B931144__INCLUDED_)
#define AFX_MDICLIENTWND_H__A19DDD02_2C26_4675_A107_262E4B931144__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MDIClientWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDIClientWnd window

class CMDIClientWnd : public CWnd
{
// Construction
public:
	CMDIClientWnd();

// Attributes
public:
protected:
	CMenu m_menuBuild, m_menuDebug, m_menuProject;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIClientWnd)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ResetMenu();
	virtual ~CMDIClientWnd();

	// Generated message map functions
protected:
	afx_msg LRESULT OnMDISetMenu(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CMDIClientWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDICLIENTWND_H__A19DDD02_2C26_4675_A107_262E4B931144__INCLUDED_)
