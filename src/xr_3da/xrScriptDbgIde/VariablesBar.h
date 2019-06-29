#pragma once

#include "TreeViewLocals.h"

struct Variable;

class CVariablesBar : public CCJTabCtrlBar//CCJControlBar  
{
public:
	void RemoveAll();
	void AddVariable(Variable*);
	CVariablesBar();
	virtual ~CVariablesBar();
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT);
	CTreeViewLocals* GetTreeViewLocals();

//protected:
//	CImageList	m_tabImages;
//	CCJListCtrl m_variables;

};

