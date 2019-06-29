// VariablesBar.cpp: implementation of the CVariablesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "VariablesBar.h"
#include "TreeViewLocals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVariablesBar::CVariablesBar()
{
}

CVariablesBar::~CVariablesBar()
{
}

BOOL CVariablesBar::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	BOOL bRet = CCJTabCtrlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
	if ( !bRet )
		return FALSE;

	AddView(_T("Local variables"),    RUNTIME_CLASS(CTreeViewLocals));

	return TRUE;
}

void CVariablesBar::AddVariable(Variable* var)
{
	CTreeViewLocals* pTree = GetTreeViewLocals();
	pTree->AddVariable(var);
	
/*	int idx = m_variables.InsertItem(m_variables.GetItemCount(), szName);
	m_variables.SetItem(idx, 1, LVIF_TEXT, szType, 0, LVIF_TEXT, LVIF_TEXT, 0);
	m_variables.SetItem(idx, 2, LVIF_TEXT, szValue, 0, LVIF_TEXT, LVIF_TEXT, 0);
*/
}

void CVariablesBar::RemoveAll()
{
//	m_variables.DeleteAllItems();
	CTreeViewLocals* pTree = GetTreeViewLocals();
	pTree->RemoveAll();
}

CTreeViewLocals* CVariablesBar::GetTreeViewLocals()
{
	return (CTreeViewLocals*)GetView(RUNTIME_CLASS(CTreeViewLocals));
}
