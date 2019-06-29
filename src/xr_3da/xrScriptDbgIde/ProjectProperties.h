#if !defined(AFX_PROJECTPROPERTIES_H__54C0E731_BB34_4ACE_9963_5A09A553BDF2__INCLUDED_)
#define AFX_PROJECTPROPERTIES_H__54C0E731_BB34_4ACE_9963_5A09A553BDF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectProperties dialog

class CProjectProperties : public CDialog
{
// Construction
public:
	CProjectProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectProperties)
	enum { IDD = IDD_PROJECT_PROPERTIES };
	BOOL	m_bGenerateListing;
	CString	m_strOutputDir;
	CString	m_strOutputPrefix;
	CString	m_strIntermediateDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectProperties)
	afx_msg void OnBrowseFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTPROPERTIES_H__54C0E731_BB34_4ACE_9963_5A09A553BDF2__INCLUDED_)
