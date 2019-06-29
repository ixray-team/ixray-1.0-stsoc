// WorkspaceWnd.cpp: implementation of the CWorkspaceWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "WorkspaceWnd.h"
#include "TreeViewFiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorkspaceWnd::CWorkspaceWnd()
{

}

CWorkspaceWnd::~CWorkspaceWnd()
{

}

BOOL CWorkspaceWnd::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	BOOL bRet = CCJTabCtrlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
	if ( !bRet )
		return FALSE;

	AddView(_T("Files"),    RUNTIME_CLASS(CTreeViewFiles));

	m_tabImages.Create (IDB_IL_TAB, 16, 1, RGB(0,255,0));
	SetTabImageList(&m_tabImages);

	Enable(FALSE);

	return TRUE;
}

CTreeViewFiles* CWorkspaceWnd::GetTreeViewFiles()
{
	return (CTreeViewFiles*)GetView(RUNTIME_CLASS(CTreeViewFiles));
}

void CWorkspaceWnd::Enable(BOOL bEnable)
{
	GetView(0)->EnableWindow(bEnable);
}
