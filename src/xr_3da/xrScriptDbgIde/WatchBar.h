// WatchBar.h: interface for the CWatchBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHBAR_H__B3588692_9C51_457B_B360_FCF2CF7336A6__INCLUDED_)
#define AFX_WATCHBAR_H__B3588692_9C51_457B_B360_FCF2CF7336A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WatchList.h"

class CWatchBar : public CCJControlBar  
{
public:
	CWatchBar();
	virtual ~CWatchBar();

	void Redraw() { m_watches.Redraw(); }
	void SetResult(int iItem, LPSTR str);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchBar)
	//}}AFX_VIRTUAL

// Generated message map functions
	void AddWatch(CString& str){m_watches.AddWatch(str);};

protected:
	CWatchList m_watches;

	//{{AFX_MSG(CWatchBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WATCHBAR_H__B3588692_9C51_457B_B360_FCF2CF7336A6__INCLUDED_)
