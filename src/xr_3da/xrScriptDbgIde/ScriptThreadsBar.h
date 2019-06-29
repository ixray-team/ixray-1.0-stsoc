#pragma once

#pragma once

struct SScriptThread;
class CScintillaView;
struct SCNotification;



class CThreadList : public CCJListCtrl
{
public:
	CThreadList(){};
	virtual ~CThreadList(){};

public:
/*
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	void Redraw();
	void UpdateRow(int iItem);
	void AddEditItem(LVITEM& item);
	void AddEmptyRow();
*/
protected:
	//{{AFX_MSG(CThreadList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



class CScriptThreadsBar : public CCJControlBar  
{
public:
	void RemoveAll();
	void AddThread(const SScriptThread* st);
	CScriptThreadsBar();
	virtual ~CScriptThreadsBar();

protected:
	CThreadList m_scriptThreads;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
