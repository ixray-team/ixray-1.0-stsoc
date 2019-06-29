#if !defined(AFX_WATCHLIST_H__4FA481A7_E054_4238_9DA3_7C729FAFC3B3__INCLUDED_)
#define AFX_WATCHLIST_H__4FA481A7_E054_4238_9DA3_7C729FAFC3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatchList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWatchList window

class CWatchList : public CCJListCtrl
{
// Construction
public:
	CWatchList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchList)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddWatch(CString& str);
	void Redraw();
	void UpdateRow(int iItem);
	void AddEditItem(LVITEM& item);
	int AddEmptyRow();
	void SetResult(int iItem, LPSTR str);

	virtual ~CWatchList();

	// Generated message map functions
protected:
	bool m_bEvalEnabled;
	CStringArray m_exps;

	//{{AFX_MSG(CWatchList)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHLIST_H__4FA481A7_E054_4238_9DA3_7C729FAFC3B3__INCLUDED_)
