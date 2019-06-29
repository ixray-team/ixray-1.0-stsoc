// WatchBar.cpp: implementation of the CWatchBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "WatchBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWatchBar, CCJControlBar)
	//{{AFX_MSG_MAP(CWatchBar)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_WATCHES, OnEndlabeledit)
//	ON_NOTIFY(LVN_DELETEITEM, IDC_WATCHES, OnDeleteItem)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CWatchBar::CWatchBar()
{

}

CWatchBar::~CWatchBar()
{

}

int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_watches.Create (WS_TABSTOP|WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0), this, IDC_WATCHES))
	{
		TRACE0("Failed to create Watches ctrl\n");
		return -1;
	}
	SetChild(&m_watches);

	m_watches.ModifyStyleEx(0, WS_EX_STATICEDGE);
	
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_watches.InsertColumn(0,&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "Value";
	lvc.cx = 300;
	lvc.fmt = LVCFMT_LEFT;
	m_watches.InsertColumn(2,&lvc);

	m_watches.AddEmptyRow();

	return 0;
}

void CWatchBar::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ( pDispInfo->item.pszText )
		m_watches.AddEditItem(pDispInfo->item);

	*pResult = 0;
}

void CWatchBar::SetResult(int iItem, LPSTR str)
{
	m_watches.SetResult(iItem, str);
}
