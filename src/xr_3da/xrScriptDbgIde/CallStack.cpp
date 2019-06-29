#include "stdafx.h"
#include "ide2.h"
#include "CallStack.h"

#include "ScintillaView.h"
#include "MainFrame.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallStack::CCallStack()
{

}

CCallStack::~CCallStack()
{

}

BOOL CCallStack::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	BOOL bRet = CCJTabCtrlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
	if ( !bRet )
		return FALSE;

	AddView(_T("Call Stack"),    RUNTIME_CLASS(CScintillaView));

	CLuaEditor* pEditor = ((CScintillaView*)GetView(0))->GetEditor();
	pEditor->SetCallStackMargins();

	Clear();
	return TRUE;
}

int CCallStack::OnSci(CScintillaView* pView, SCNotification* pNotify)
{
	CLuaEditor* pEditor = ((CScintillaView*)GetView(0))->GetEditor();

//	CScintillaCtrl& rCtrl = (CScintillaView*)GetView(0)->GetCtrl();
	CPoint pt;
	int nLine;
//	CString strLine;
	switch (pNotify->nmhdr.code)
	{
		case SCN_DOUBLECLICK:
			GetCursorPos(&pt);
			pEditor->ScreenToClient(&pt);
			nLine = pEditor->LineFromPoint(pt);
			GotoStackTraceLevel(nLine-1);

//			int nPos = rCtrl.GetCurrentPos();
//			int nLine = rCtrl.LineFromPosition(nPos);
//			GotoStackTraceLevel(nLine-1);

		break;
	};

	return 0;
}

void CCallStack::Clear()
{
	((CScintillaView*)GetView(0))->Clear();

	m_nCurrentLevel = -1;
	m_lines.RemoveAll();
	m_files.RemoveAll();
}

void CCallStack::Add(const char *szDesc, const char *szFile, int nLine)
{
	((CScintillaView*)GetView(0))->Write(CString(szDesc)+"\n");

	m_files.Add(szFile);
	m_lines.Add(nLine);
}

void CCallStack::GotoStackTraceLevel(int nLevel)
{
	if ( nLevel<0 || nLevel>=m_files.GetSize() )
		return;

	m_nCurrentLevel = nLevel;

	CLuaEditor* pEditor = ((CScintillaView*)GetView(0))->GetEditor();
	pEditor->SetStackTraceLevel(nLevel);

	g_mainFrame->GotoFileLine(m_files[nLevel], m_lines[nLevel]);
	g_mainFrame->StackLevelChanged(nLevel);
}
