// OutputWnd.cpp: implementation of the COutputWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "OutputWnd.h"
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

COutputWnd::COutputWnd()
{

}

COutputWnd::~COutputWnd()
{

}

BOOL COutputWnd::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle, LPCSTR caption)
{
	BOOL bRet = CCJTabCtrlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
	if ( !bRet )
		return FALSE;

//	AddView(_T("Build"),    RUNTIME_CLASS(CScintillaView));
	AddView(caption,    RUNTIME_CLASS(CScintillaView));
//	AddView(_T("Find in Files"),    RUNTIME_CLASS(CScintillaView));
//	AddView(_T("Lua runtime"),    RUNTIME_CLASS(CRichEditView));

	return TRUE;
}

int COutputWnd::OnSci(CScintillaView* pView, SCNotification* pNotify)
{
/*	if ( pView==GetView(outputBuild) )
		return OnBuildSci(pNotify);
	else if ( pView==GetView(outputDebug) )
		return OnDebugSci(pNotify);
*/	
	if ( pView==GetView(outputDebug) )
		return OnDebugSci(pNotify);

	return 0;
}

int COutputWnd::OnBuildSci(SCNotification* pNotify)
{
/*	CLuaEditor* pEditor = GetOutput(outputBuild)->GetEditor();

	CPoint pt;
	int nLine;
	CString strLine;
	switch (pNotify->nmhdr.code)
	{
		case SCN_DOUBLECLICK:
			GetCursorPos(&pt);
			pEditor->ScreenToClient(&pt);
			nLine = pEditor->LineFromPoint(pt);
			strLine = pEditor->GetLine(nLine);
			GotoLine(strLine);
		break;
	};
*/
	return 0;
}

int COutputWnd::OnDebugSci(SCNotification* pNotify)
{
	CLuaEditor* pEditor = GetOutput(outputDebug)->GetEditor();

	CPoint pt;
	int nLine;
	CString strLine;
	switch (pNotify->nmhdr.code)
	{
		case SCN_DOUBLECLICK:
			GetCursorPos(&pt);
			pEditor->ScreenToClient(&pt);
			nLine = pEditor->LineFromPoint(pt);
			strLine = pEditor->GetLine(nLine);
			GotoLine(strLine);
		break;
	};

	return 0;
}

void COutputWnd::GotoLine(CString strLine)
{
	CString strPathName;
	int nLine;

	if ( !g_mainFrame->ErrorStringToFileLine(strLine, strPathName, nLine) )
		return;

	g_mainFrame->GotoFileLine(strPathName, nLine);	

}
