// LuaFrame.cpp : implementation of the CLuaFrame class
//

#include "stdafx.h"
#include "ide2.h"

#include "LuaFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLuaFrame

IMPLEMENT_DYNCREATE(CLuaFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CLuaFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CLuaFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLuaFrame construction/destruction

CLuaFrame::CLuaFrame()
{
	// TODO: add member initialization code here
	
}

CLuaFrame::~CLuaFrame()
{
}

BOOL CLuaFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLuaFrame diagnostics

#ifdef _DEBUG
void CLuaFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CLuaFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLuaFrame message handlers

BOOL CLuaFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::PreTranslateMessage(pMsg);
}
