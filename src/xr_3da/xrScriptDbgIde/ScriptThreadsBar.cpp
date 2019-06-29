#include "stdafx.h"
#include "ide2.h"
#include "ScriptThreadsBar.h"
#include "MainFrame.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CScriptThreadsBar, CCJControlBar)
	//{{AFX_MSG_MAP(CVariablesBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CScriptThreadsBar::CScriptThreadsBar()
{}

CScriptThreadsBar::~CScriptThreadsBar()
{}

int CScriptThreadsBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_scriptThreads.Create (WS_TABSTOP|WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0), this, IDC_THREADS))
	{
		TRACE0("Failed to create Threads Bar ctrl\n");
		return -1;
	}
	SetChild(&m_scriptThreads);

	m_scriptThreads.ModifyStyleEx(0, WS_EX_STATICEDGE);

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "NameSpace";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	m_scriptThreads.InsertColumn(0,&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "State";
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
	m_scriptThreads.InsertColumn(1,&lvc);

	lvc.iSubItem = 2;
	lvc.pszText = "ID";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
	m_scriptThreads.InsertColumn(2,&lvc);

	lvc.iSubItem = 3;
	lvc.pszText = "Processor";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_LEFT;
	m_scriptThreads.InsertColumn(3,&lvc);
	return 0;
}

void CScriptThreadsBar::AddThread(const SScriptThread* st)
{
	int idx = m_scriptThreads.InsertItem(m_scriptThreads.GetItemCount(), st->name);
	m_scriptThreads.SetItem(idx, 1, LVIF_TEXT, st->active?"Active":"Not Active", 0, LVIF_TEXT, LVIF_TEXT, 0);
	CString sID;
	sID.Format("%d",st->scriptID);
	m_scriptThreads.SetItem(idx, 2, LVIF_TEXT, sID, 0, LVIF_TEXT, LVIF_TEXT, 0);
	m_scriptThreads.SetItem(idx, 3, LVIF_TEXT, st->process, 0, LVIF_TEXT, LVIF_TEXT, 0);
}

void CScriptThreadsBar::RemoveAll()
{
	m_scriptThreads.DeleteAllItems();
}


//------------------CThreadList
BEGIN_MESSAGE_MAP(CThreadList, CCJListCtrl)
	//{{AFX_MSG_MAP(CWatchList)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CThreadList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);

	if (lvhti.flags & LVHT_ONITEMLABEL){
		CString num = GetItemText(lvhti.iItem,2);
		int nThreadID=0;
		int res = sscanf(num,"%d",&nThreadID);
		ASSERT(res!=EOF);

		g_mainFrame->ThreadChanged(nThreadID);
	};
CCJListCtrl::OnLButtonDown(nFlags, point);

}
