// WatchList.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "WatchList.h"

#include "MainFrame.h"
#include "winuser.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchList

CWatchList::CWatchList()
{
	m_bEvalEnabled = true;
}

CWatchList::~CWatchList()
{
}


BEGIN_MESSAGE_MAP(CWatchList, CCJListCtrl)
	//{{AFX_MSG_MAP(CWatchList)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchList message handlers

int CWatchList::AddEmptyRow()
{
	int nItem = InsertItem(GetItemCount(), "");
	SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	return nItem;
}

BOOL CWatchList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_EDITLABELS|LVS_NOSORTHEADER|LVS_SHOWSELALWAYS|LVS_SINGLESEL;

	return CCJListCtrl::PreCreateWindow(cs);
}

void CWatchList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();

	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);

	if (lvhti.flags & LVHT_ONITEMLABEL){
		m_bEvalEnabled = false;
		EditLabel(lvhti.iItem);


	}else
		CCJListCtrl::OnLButtonDblClk(nFlags, point);
}
void CWatchList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!(nChar == 46 || nChar==110) )//del
		return;


POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	while (pos)
	{
		int nItem = GetNextSelectedItem(pos);
		DeleteItem(nItem);
	};

	if(GetItemCount() == 0)
		AddEmptyRow();

}

void CWatchList::AddEditItem(LVITEM &item)
{
	if ( item.iItem == GetItemCount()-1 )
	{
		if ( strlen(item.pszText) == 0 )
			return;

		m_exps.Add(item.pszText);
		AddEmptyRow();
		UpdateRow(item.iItem);
	}
	else
	{
		if ( strlen(item.pszText) == 0 )
		{
			DeleteItem(item.iItem);
			m_exps.RemoveAt(item.iItem);
			return;
		}
		else
		{
			m_exps[item.iItem] = item.pszText;
			UpdateRow(item.iItem);
		}
	}
	m_bEvalEnabled = true;
}

void CWatchList::UpdateRow(int iItem)
{
	SetItemText(iItem, 0, m_exps[iItem]);

	CMainFrame* pFrame = g_mainFrame;

	pFrame->EvalWatch(m_exps[iItem], iItem);
//	if ( pFrame->GetMode() == CMainFrame::modeDebugBreak )
//	{
//		SetItemText(iItem, 1, pFrame->GetDebugger()->Eval(m_exps[iItem]));
//	}
}


void CWatchList::Redraw()
{
	if (!m_bEvalEnabled)
		return;

	CMainFrame* pFrame = g_mainFrame;
	for ( int i=0; i<m_exps.GetSize(); ++i )
//		SetItemText(i, 1, pFrame->GetDebugger()->Eval(m_exps[i]));
	pFrame->EvalWatch(m_exps[i],i);

}
void CWatchList::SetResult(int iItem, LPSTR str)
{
	if (!m_bEvalEnabled)
		return;
	SetItemText(iItem, 1, str);
}

void CWatchList::AddWatch(CString& str)
{
		INT_PTR i = m_exps.Add(str);
		int row = AddEmptyRow();
		SetItemText(row-1, 0, str);
		UpdateRow(row-1);
}
