// LuaView.cpp : implementation of the CLuaView class
//

#include "stdafx.h"
#include "ide2.h"

#include "LuaDoc.h"
#include "LuaView.h"
#include "ProjectFile.h"
#include "MainFrame.h"
#include "ScintillaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLuaView

IMPLEMENT_DYNCREATE(CLuaView, CView)

const UINT _ScintillaMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CLuaView, CView)
	//{{AFX_MSG_MAP(CLuaView)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_COMMAND(ID_ADD_WATCH, OnAddWatch)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_TOGGLE_BREAKPOINT, OnToggleBreakpoint)

	ON_COMMAND(ID_EDIT_GOTOLINE, OnGotoLineNumber)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GOTOLINE, OnUpdateGotoLineNumber)

	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_FINDNEXT, OnEditFindNext)

	ON_UPDATE_COMMAND_UI(ID_EDIT_GOTOLINE, OnUpdateGotoLineNumber)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FINDNEXT, OnUpdateFind)

	ON_COMMAND(ID_SHOW_FUNCTION_LIST, OnFunctionList)
	ON_COMMAND(ID_EDIT_COMPLETEWORD, OnCompleteWord)

	ON_COMMAND(ID_EDIT_TOGGLEBOOKMARK, OnToggleBookMark)
	ON_COMMAND(ID_EDIT_GOTONEXTBOOKMARK, OnNextBookMark)
	
	ON_COMMAND(ID_EDIT_BREAKPOINTSLIST, OnBreakPointList)
		

	
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(_ScintillaMsgFindReplace, OnFindReplaceCmd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLuaView construction/destruction

CLuaView::CLuaView()
{
	m_pFindReplaceDlg = NULL;
	m_bFindOnly = TRUE;
	m_bCase = FALSE;
	m_bNext = TRUE;
	m_bWord = FALSE;
	m_bFirstSearch = TRUE;
	m_bRegularExpression = FALSE;
	
	SetProjectFile(NULL);
}

CLuaView::~CLuaView()
{
	if(GetProjectFile() )
		GetProjectFile()->SetLuaView( NULL );
}

CScintillaCtrl& CLuaView::GetCtrl()
{ 
  CScintillaCtrl& ctrl = m_Edit;

  return ctrl; 
}


BOOL CLuaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLuaView drawing

void CLuaView::OnDraw(CDC* pDC)
{
	CLuaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CLuaView diagnostics

#ifdef _DEBUG
void CLuaView::AssertValid() const
{
	CView::AssertValid();
}

void CLuaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLuaDoc* CLuaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLuaDoc)));
	return (CLuaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLuaView message handlers

int CLuaView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !m_editor.Create(this, ID_EDIT_CTRL) )
		return -1;

	
	m_editor.SetEditorMargins();
//	m_editor.SetLuaLexer();
	m_editor.setupLexer_ex();

/*	CRect r;
	if (!m_Edit.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, r, this, 0))
		return -1;
*/
	return 0;
}

void CLuaView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( ::IsWindow(m_editor.m_hWnd) )
		m_editor.SetWindowPos(NULL, 0, 0, cx, cy, 0);


/*	CRect r;
	GetClientRect(&r);
	m_Edit.MoveWindow(&r);	
*/
}

BOOL CLuaView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *lpnmhdr = (LPNMHDR) lParam; 
	if (lpnmhdr->hwndFrom == m_editor.m_hWnd)
	{
		*pResult = OnSci((SCNotification*)lParam);
		return CView::OnNotify(wParam, lParam, pResult);
		
//		return TRUE;
	}
	return CView::OnNotify(wParam, lParam, pResult);
}

int CLuaView::OnSci(SCNotification* pNotify)
{
	CPoint pt;
	int nLine;

	switch (pNotify->nmhdr.code)
	{
		case SCN_USERLISTSELECTION:{
			int type = pNotify->wParam;
			int idx = pNotify->lParam;
			switch (type){
		case eWordList:{
	   		GetEditor()->CompleteWord(pNotify->text);

					   }break;
		case eFuncList:{
			theApp.OpenProjectFilesView(g_funcArray[idx]->pf);
			g_funcArray[idx]->pf->GetLuaView()->GetEditor()->GotoLine(g_funcArray[idx]->line_num);
					   }break;
		case eBreakPointList:{
			theApp.OpenProjectFilesView(g_breakpointsArray[idx]->pf);
			g_breakpointsArray[idx]->pf->GetLuaView()->GetEditor()->GotoLine(g_breakpointsArray[idx]->line_num);
						}break;
			}

		}break;
		case SCN_MARGINCLICK:
			if ( GetDocument()->IsInProject() )
			{
				GetCursorPos(&pt);
				ScreenToClient(&pt);
				nLine = m_editor.LineFromPoint(pt);
				
				ToggleBreakPoint(nLine);
			}break;

		case SCN_SAVEPOINTREACHED:
			GetDocument()->SetModifiedFlag(FALSE);
		break;

		case SCN_SAVEPOINTLEFT:
			GetDocument()->SetModifiedFlag(TRUE);
		break;

		case SCN_MODIFYATTEMPTRO:
		{
			if( GetEditor()->GetReadOnly() ){
				if( (GetProjectFile()->GetSS_status()== vss_not_checked_out) ||
					(GetProjectFile()->GetSS_status()== vss_checked_out)){
						if( IDYES==AfxMessageBox("Check Out for edit ?",MB_YESNO) ){
							GetProjectFile()->SS_check_out();
						}
					}else
			AfxMessageBox("File is read only");
			}

		}
		case SCN_CHARADDED:
		{

			if(pNotify->ch==13){
				GetEditor()->AdjustCurrTabIndent();
			}

			if( GetEditor()->Sci(SCI_AUTOCACTIVE) )
				OnCompleteWord();
		}break;

	}

	return TRUE;
}

void CLuaView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_editor.Copy();
}

void CLuaView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditCut() 
{
	m_editor.Cut();
}

void CLuaView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditClear() 
{
	m_editor.Clear();
}

void CLuaView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditPaste() 
{
	m_editor.Paste();
}

void CLuaView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanPaste());
}

void CLuaView::OnEditUndo() 
{
	m_editor.Undo();
}

void CLuaView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_editor.CanUndo());
}

void CLuaView::OnUpdateGotoLineNumber(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
void CLuaView::OnUpdateFind(CCmdUI* pCmdUI)
{
	long sz = GetEditor()->GetLength();
	pCmdUI->Enable(sz>0);

	if(pCmdUI->m_nID == ID_EDIT_FINDNEXT){
		pCmdUI->Enable(m_strFind.GetLength()>0);
	}
	
}

void CLuaView::OnEditRedo() 
{
	m_editor.Redo();
}

void CLuaView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanRedo());	
}

void CLuaView::OnEditSelectAll() 
{
	m_editor.SelectAll();	
}


void CLuaView::Activate()
{
	CFrameWnd* pFrame = GetParentFrame();

	if (pFrame != NULL)
		pFrame->ActivateFrame();
	else
		TRACE0("Error: Can not find a frame for document to activate.\n");

	CFrameWnd* pAppFrame;
	if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
	{
		ASSERT_KINDOF(CFrameWnd, pAppFrame);
		pAppFrame->ActivateFrame();
	}

	GetEditor()->GrabFocus();
}


void CLuaView::CloseFrame()
{
	CFrameWnd* pFrame = GetParentFrame();

	if (pFrame != NULL)
		pFrame->SendMessage(WM_CLOSE);
}

void CLuaView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// use main frame dynamic accel table
	GetParentFrame()->m_hAccelTable = NULL;
}

void CLuaView::ToggleBreakPoint(int nLine)
{
	CMainFrame* pFrame = g_mainFrame;
/*	if ( pFrame->GetMode() == CMainFrame::modeDebug ||
		pFrame->GetMode() == CMainFrame::modeDebugBreak )
	{
		nLine = GetDocument()->GetProjectFile()->GetNearestDebugLine(nLine);
		if ( nLine==0 )
			return;
	}*/

	if ( m_editor.ToggleBreakpoint(nLine) )
		GetDocument()->GetProjectFile()->AddBreakPoint(nLine);
	else
		GetDocument()->GetProjectFile()->RemoveBreakPoint(nLine);
}

void CLuaView::OnToggleBookMark()
{
		m_editor.SetBookMark( m_editor.GetCurrentLine() );
}

void CLuaView::OnNextBookMark()
{
		m_editor.GotoNextBookMark( m_editor.GetCurrentLine() );
}

void CLuaView::OnToggleBreakpoint() 
{
	if ( GetDocument()&&GetDocument()->IsInProject() )
		ToggleBreakPoint(m_editor.GetCurrentLine());
}
void CLuaView::OnGotoLineNumber()
{
	m_editor.GotoLineDlg();
}


void CLuaView::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{

		CLuaEditor* pEditor = GetEditor();
		pEditor->Save(ar.GetFile());
		if( GetProjectFile() )
			pEditor->SetBreakPointsIn( GetProjectFile() );

	}
	else
	{
		CLuaEditor* pEditor = GetEditor();
		pEditor->Load(ar.GetFile());
	}
}
///////////////////////////////////////////////////////////////////////////////////
void CLuaView::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);

	m_strFind = lpszFind;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = bNext;
  m_bRegularExpression = bRegularExpression;

	if (!FindText(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);
	else
		AdjustFindDialogPosition();
	ASSERT_VALID(this);
}

void CLuaView::TextNotFound(LPCTSTR /*lpszFind*/, BOOL /*bNext*/, BOOL /*bCase*/, BOOL /*bWord*/, BOOL /*bRegularExpression*/)
{
	ASSERT_VALID(this);
	m_bFirstSearch = TRUE;
	AfxMessageBox("Text not found");
//	MessageBeep(MB_ICONHAND);
}

BOOL CLuaView::FindText(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);
	CWaitCursor wait;
	return FindTextSimple(lpszFind, bNext, bCase, bWord, bRegularExpression);
}

void CLuaView::OnEditFindReplace(BOOL bFindOnly)
{
	ASSERT_VALID(this);

	m_bFirstSearch = TRUE;
	if (m_pFindReplaceDlg != NULL)
	{
		if (m_bFindOnly == bFindOnly)
		{
			m_pFindReplaceDlg->SetActiveWindow();
			m_pFindReplaceDlg->ShowWindow(SW_SHOW);
			return;
		}
		else
		{
			ASSERT(m_bFindOnly != bFindOnly);
			m_pFindReplaceDlg->SendMessage(WM_CLOSE);
			ASSERT(m_pFindReplaceDlg == NULL);
			ASSERT_VALID(this);
		}
	}
//  CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();
	CString strFind = rCtrl.GetSelText();
	//if selection is empty or spans multiple lines use old find text
	if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
		strFind = m_strFind;

	CString strReplace = m_strReplace;
	m_pFindReplaceDlg = CreateFindReplaceDialog();
	ASSERT(m_pFindReplaceDlg != NULL);
	DWORD dwFlags = NULL;
	if (m_bNext)
		dwFlags |= FR_DOWN;
	if (m_bCase)
		dwFlags |= FR_MATCHCASE;
	if (m_bWord)
		dwFlags |= FR_WHOLEWORD;
  if (m_bRegularExpression)
    m_pFindReplaceDlg->SetRegularExpression(TRUE);

	if (!m_pFindReplaceDlg->Create(bFindOnly, strFind, strReplace, dwFlags, this))
	{
		m_pFindReplaceDlg = NULL;
		ASSERT_VALID(this);
		return;
	}
	ASSERT(m_pFindReplaceDlg != NULL);
	m_bFindOnly = bFindOnly;
	m_pFindReplaceDlg->SetActiveWindow();
	m_pFindReplaceDlg->ShowWindow(SW_SHOW);
	ASSERT_VALID(this);
}

BOOL CLuaView::FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	USES_CONVERSION;

//  CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();
	ASSERT(lpszFind != NULL);
	TextToFind ft;
  ft.chrg.cpMin = rCtrl.GetSelectionStart();
  ft.chrg.cpMax = rCtrl.GetSelectionEnd();
	if (m_bFirstSearch)
		m_bFirstSearch = FALSE;

	ft.lpstrText = T2A((LPTSTR)lpszFind);
	if (ft.chrg.cpMin != ft.chrg.cpMax) // i.e. there is a selection
	{
#ifndef _UNICODE
		// If byte at beginning of selection is a DBCS lead byte,
		// increment by one extra byte.
		TEXTRANGE textRange;
		TCHAR ch[2];
		textRange.chrg.cpMin = ft.chrg.cpMin;
		textRange.chrg.cpMax = ft.chrg.cpMin + 1;
		textRange.lpstrText = ch;
		rCtrl.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange);
		if (_istlead(ch[0]))
		{
			ASSERT(ft.chrg.cpMax - ft.chrg.cpMin >= 2);

      if (bNext)
  			ft.chrg.cpMin++;
      else
        ft.chrg.cpMax = ft.chrg.cpMin - 1;
		}
#endif

    if (bNext)  
		  ft.chrg.cpMin++;
    else
      ft.chrg.cpMax = ft.chrg.cpMin - 1;
	}

  int nLength = rCtrl.GetLength();
  if (bNext)
    ft.chrg.cpMax = nLength;
	else
 	  ft.chrg.cpMin = 0;

	DWORD dwFlags = bCase ? SCFIND_MATCHCASE : 0;
	dwFlags |= bWord ? SCFIND_WHOLEWORD : 0;
  dwFlags |= bRegularExpression ? SCFIND_REGEXP : 0;

  if (!bNext)
  {
    //Swap the start and end positions which Scintilla uses to flag backward searches
    int ncpMinTemp = ft.chrg.cpMin;
    ft.chrg.cpMin = ft.chrg.cpMax;
    ft.chrg.cpMax = ncpMinTemp;
  }

	// if we find the text return TRUE
	return (FindAndSelect(dwFlags, ft) != -1);
}

void CLuaView::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase,	BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace)
{
	ASSERT_VALID(this);

	m_strFind = lpszFind;
	m_strReplace = lpszReplace;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = bNext;
  m_bRegularExpression = bRegularExpression;

	if (!SameAsSelected(m_strFind, m_bCase, m_bWord, m_bRegularExpression))
	{
		if (!FindText(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
			TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);
		else
			AdjustFindDialogPosition();
		return;
	}
	CLuaEditor& rCtrl = *GetEditor();
	rCtrl.ReplaceSel(m_strReplace);
	if (!FindText(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);
	else
		AdjustFindDialogPosition();
	ASSERT_VALID(this);
}

void CLuaView::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
	ASSERT_VALID(this);

	m_strFind = lpszFind;
	m_strReplace = lpszReplace;
	m_bCase = bCase;
	m_bWord = bWord;
	m_bNext = TRUE;
  m_bRegularExpression = bRegularExpression;

	CWaitCursor wait;
	// no selection or different than what looking for
	if (!SameAsSelected(m_strFind, m_bCase, m_bWord, bRegularExpression))
	{
		if (!FindText(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		{
			TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);
			return;
		}
	}

//	CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();
	rCtrl.HideSelection(TRUE, FALSE);
	do
	{
		rCtrl.ReplaceSel(m_strReplace);
	} 
  while (FindTextSimple(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression));

	TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);
	rCtrl.HideSelection(FALSE, FALSE);

	ASSERT_VALID(this);
}

BOOL CLuaView::SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL /*bWord*/, BOOL bRegularExpression)
{
  //if we are doing a regular expression Find / Replace, then it they match!!
  if (bRegularExpression)
    return TRUE;

	// check length first
	size_t nLen = lstrlen(lpszCompare);
//  CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();

  int nStartChar = rCtrl.GetSelectionStart();
  int nEndChar = rCtrl.GetSelectionEnd();
	if (nLen != (size_t)(nEndChar - nStartChar))
		return FALSE;

	// length is the same, check contents
	CString strSelect = rCtrl.GetSelText();
	return (bCase && lstrcmp(lpszCompare, strSelect) == 0) || (!bCase && lstrcmpi(lpszCompare, strSelect) == 0);
}

long CLuaView::FindAndSelect(DWORD dwFlags, TextToFind& ft)
{
//	CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();
	long index = rCtrl.FindText(dwFlags, &ft);
	if (index != -1) // i.e. we found something
    rCtrl.SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
	return index;
}

void CLuaView::AdjustFindDialogPosition()
{
	ASSERT(m_pFindReplaceDlg);
//  CScintillaCtrl& rCtrl = GetCtrl();
	CLuaEditor& rCtrl = *GetEditor();
  int nStart = rCtrl.GetSelectionStart();
	CPoint point;
  point.x = rCtrl.PointXFromPosition(nStart);
  point.y = rCtrl.PointYFromPosition(nStart);
	ClientToScreen(&point);
	CRect rectDlg;
	m_pFindReplaceDlg->GetWindowRect(&rectDlg);
	if (rectDlg.PtInRect(point))
	{
		if (point.y > rectDlg.Height())
			rectDlg.OffsetRect(0, point.y - rectDlg.bottom - 20);
		else
		{
			int nVertExt = GetSystemMetrics(SM_CYSCREEN);
			if (point.y + rectDlg.Height() < nVertExt)
				rectDlg.OffsetRect(0, 40 + point.y - rectDlg.top);
		}
		m_pFindReplaceDlg->MoveWindow(&rectDlg);
	}
}

CScintillaFindReplaceDlg* CLuaView::CreateFindReplaceDialog()
{
  return new CScintillaFindReplaceDlg;
}

void CLuaView::OnEditFind()
{
	ASSERT_VALID(this);
	OnEditFindReplace(TRUE);
}

void CLuaView::OnEditReplace()
{
	ASSERT_VALID(this);
	OnEditFindReplace(FALSE);
}

void CLuaView::OnEditFindNext()
{
	ASSERT_VALID(this);
	if (!FindText(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression))
		TextNotFound(m_strFind, m_bNext, m_bCase, m_bWord, m_bRegularExpression);

}


LRESULT CLuaView::OnFindReplaceCmd(WPARAM /*wParam*/, LPARAM lParam)
{
	ASSERT_VALID(this);

	CScintillaFindReplaceDlg* pDialog = (CScintillaFindReplaceDlg*) CFindReplaceDialog::GetNotifier(lParam);
	ASSERT(pDialog != NULL);
	ASSERT(pDialog == m_pFindReplaceDlg);

	if (pDialog->IsTerminating())
		m_pFindReplaceDlg = NULL;
	else if (pDialog->FindNext())
		OnFindNext(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
	else if (pDialog->ReplaceCurrent())
	{
		ASSERT(!m_bFindOnly);
		OnReplaceSel(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression(), pDialog->GetReplaceString());
	}
	else if (pDialog->ReplaceAll())
	{
		ASSERT(!m_bFindOnly);
		OnReplaceAll(pDialog->GetFindString(), pDialog->GetReplaceString(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
	}
	ASSERT_VALID(this);

	return 0;
}

bool CLuaView::_save()
{
	if( GetProjectFile() ){

		CFileStatus fs;
		CFile::GetStatus(GetProjectFile()->GetPathName(),fs);
		if (fs.m_attribute & CFile::readOnly){
			CString str;
			str.Format("Cannot save file %s\n The file have read-only status.",GetProjectFile()->GetPathName());
			AfxMessageBox(str);
			return false;
		}

		CFile ff;
		ff.Open(GetProjectFile()->GetPathName(), CFile::modeCreate |
			CFile::modeWrite | CFile::shareExclusive);

	CArchive saveArchive(&ff, CArchive::store | CArchive::bNoFlushOnDelete);
	
	Serialize(saveArchive);
	return true;
	}
	return false;
}

void CLuaView::OnCompleteWord()
{
	int word_end = GetEditor()->GetSelectionEnd();

	int word_start = GetEditor()->GetWordStart(word_end);

	char buff_[2048];
	TextRange tr;
	tr.lpstrText = buff_;
	tr.chrg.cpMin = word_start;
	tr.chrg.cpMax = word_end;
	GetEditor()->Sci(SCI_GETTEXTRANGE,0,(int)&tr);
	char str_word[2048];
	strcpy(str_word,tr.lpstrText);
	if(strlen(str_word)<3)
		return;

	CString buff;
	g_mainFrame->GetProject()->CreateWordList(str_word,buff);

	if(buff.GetLength() )
		GetEditor()->Sci(SCI_AUTOCSHOW, strlen(str_word), (int)buff.GetBuffer());
	else
		if( GetEditor()->Sci(SCI_AUTOCACTIVE) )
			GetEditor()->Sci(SCI_AUTOCCANCEL);

	return;

}

void CLuaView::OnFunctionList()
{

	free(g_breakpointsArray);

	CString buff;
	g_mainFrame->GetProject()->CreateFunctionsList(buff);

	if(buff.GetLength() )
		GetEditor()->Sci(SCI_USERLISTSHOW, eFuncList, (int)buff.GetBuffer());

	return;
}

void CLuaView::OnBreakPointList()
{
	free(g_breakpointsArray);

	CString buff;
	g_mainFrame->GetProject()->CreateBreakPointList(buff);

	if(buff.GetLength() )
		GetEditor()->Sci(SCI_USERLISTSHOW, eBreakPointList, (int)buff.GetBuffer());

	return;
}



void CLuaView::OnAddWatch() 
{
	CMenu mnu;
	mnu.CreatePopupMenu();
	CString sel = GetEditor()->GetSelText();
	if(sel.GetLength()==0)
		return;

	mnu.AppendMenu(MF_STRING,1,"Add watch");

	RECT rct;
	GetWindowRect(&rct);
	rct.left += GetEditor()->PointXFromPosition( GetEditor()->GetSelectionEnd() );
	rct.top  += GetEditor()->PointYFromPosition( GetEditor()->GetSelectionEnd() );

	::SetForegroundWindow(m_hWnd);	
	int idx = mnu.TrackPopupMenuEx(TPM_RETURNCMD, rct.left, rct.top, this, NULL); 
	if( 1 == idx ){ //add to watchBar
		g_mainFrame->m_wndWatches.AddWatch(sel);
	}

}

void CLuaView::OnActivateView(  BOOL bActivate,  CView* pActivateView,  CView* pDeactiveView )
{
	CString msg;
	if( bActivate && (this==pActivateView) )
	{
		GetEditor()->GrabFocus();
	};

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CLuaView::OnActivateFrame(UINT nState, CFrameWnd* pFrameWnd)
{
	int i=0;
	CView::OnActivateFrame(nState, pFrameWnd);
}
