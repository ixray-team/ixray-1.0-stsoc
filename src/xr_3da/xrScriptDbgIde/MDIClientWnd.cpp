// MDIClientWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "MDIClientWnd.h"

#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIClientWnd

CMDIClientWnd::CMDIClientWnd()
{
}

CMDIClientWnd::~CMDIClientWnd()
{
}


BEGIN_MESSAGE_MAP(CMDIClientWnd, CWnd)
	//{{AFX_MSG_MAP(CMDIClientWnd)
	ON_MESSAGE(WM_MDISETMENU, OnMDISetMenu)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMDIClientWnd message handlers

LRESULT CMDIClientWnd::OnMDISetMenu(WPARAM wParam, LPARAM lParam)
{
	Default();

	ResetMenu();

	return 0;
}

void CMDIClientWnd::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	VERIFY(m_menuBuild.LoadMenu(IDR_SUBMENU_BUILD));
	VERIFY(m_menuDebug.LoadMenu(IDR_SUBMENU_DEBUG));
	VERIFY(m_menuProject.LoadMenu(IDR_PROJECT_MENU));
	
	CWnd::PreSubclassWindow();
}

BOOL CMDIClientWnd::OnEraseBkgnd(CDC* pDC) 
{
/*      CBrush backBrush(RGB(58, 111, 165));

      // Save old brush
      CBrush* pOldBrush = pDC->SelectObject(&backBrush);

      CRect rect;
      pDC->GetClipBox(&rect);     // Erase the area needed

      pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
          PATCOPY);
      pDC->SelectObject(pOldBrush);
      return TRUE;
*/	
	return CWnd::OnEraseBkgnd(pDC);
}

void CMDIClientWnd::ResetMenu()
{
	CMenu* pMenu = g_mainFrame->GetMenu();

	CString strMenu;
	int nMenuItemCount = pMenu->GetMenuItemCount();

	for ( int i=0; i<nMenuItemCount; ++i )
	{
		pMenu->GetMenuString(i, strMenu, MF_BYPOSITION);
		if ( strMenu=="&Build" || strMenu=="&Debug" || strMenu=="&Project")
		{
			pMenu->RemoveMenu(i, MF_BYPOSITION);
			--i;
			--nMenuItemCount;
		}
	}

	pMenu->GetMenuString(1, strMenu, MF_BYPOSITION);
	int nPos = 2;
	if ( strMenu=="&Edit" )
		++nPos;

	CMainFrame* pFrame = g_mainFrame;
	if ( pFrame->GetMode() != pFrame->modeNoProject )
	{
		pMenu->InsertMenu(nPos, MF_BYPOSITION|MF_POPUP|MF_ENABLED, 
			(UINT)m_menuProject.GetSubMenu(0)->GetSafeHmenu(), "&Project");
		++nPos;
	}

/*	if ( pFrame->GetMode() == pFrame->modeIdle )
		pMenu->InsertMenu(nPos, MF_BYPOSITION|MF_POPUP|MF_ENABLED, 
			(UINT)m_menuBuild.GetSubMenu(0)->GetSafeHmenu(), "&Build");
	else */
		if (pFrame->GetMode() == pFrame->modeIdle || pFrame->GetMode()==pFrame->modeDebug || pFrame->GetMode()==pFrame->modeDebugBreak )
		pMenu->InsertMenu(nPos, MF_BYPOSITION|MF_POPUP|MF_ENABLED, 
			(UINT)m_menuDebug.GetSubMenu(0)->GetSafeHmenu(), "&Debug");
}
