// LuaEditor.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "LuaEditor.h"

#include "ProjectFile.h"
#include "SString.h"
#include "SciLexer.h"
#include "MainFrame.h"
#include "GotoLineDialog.h"
#include "LuaView.h"
#include "LuaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWordListArray				g_wordArray;
CFuncListArray				g_funcArray;
CBreakpointListArray		g_breakpointsArray;
/////////////////////////////////////////////////////////////////////////////
// CLuaEditor

void CLuaEditor::GotoLineDlg()
{
	CGotoLineDialog dlg;
	if ( dlg.DoModal()!=IDOK )
		return ;

	int ln = dlg.m_line_n;
	GotoLine(ln);
}

CLuaEditor::CLuaEditor()
{
	m_bShowCalltips = TRUE;
	m_lua_view	= NULL;
}

CLuaEditor::~CLuaEditor()
{
}


BEGIN_MESSAGE_MAP(CLuaEditor, CWnd)
	//{{AFX_MSG_MAP(CLuaEditor)
	ON_WM_MOUSEMOVE()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CLuaEditor message handlers

BOOL CLuaEditor::Create(CWnd *pParentWnd, UINT nCtrlId)
{
	BOOL bCreated = CreateEx(0, "Scintilla","", WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		CRect(0,0,0,0),pParentWnd,nCtrlId);
	
	m_lua_view = (CLuaView*)pParentWnd;

	if ( !bCreated )
		return FALSE;

	m_fnScintilla = (int (__cdecl *)(void *,int,int,int))
		SendMessage(SCI_GETDIRECTFUNCTION,0,0);
	m_ptrScintilla = (void *)SendMessage(SCI_GETDIRECTPOINTER,0,0);

	Sci(SCI_SETMARGINWIDTHN, 1, 0);

	return TRUE;
}

int CLuaEditor::Sci(int nCmd, int wParam, int lParam)
{
	ASSERT(m_fnScintilla);
	ASSERT(m_ptrScintilla);

	static int nLineTotal = 0;
	int cnt = m_fnScintilla(m_ptrScintilla, SCI_GETLINECOUNT, 0, 0);
	
	return m_fnScintilla(m_ptrScintilla, nCmd, wParam, lParam);
}

int CLuaEditor::LineFromPoint(CPoint &pt)
{
	return 1+Sci(SCI_LINEFROMPOSITION, Sci(SCI_POSITIONFROMPOINT, pt.x, pt.y), 0);
}

BOOL CLuaEditor::ToggleBreakpoint(int nLine)
{
	if ( Sci(SCI_MARKERGET, nLine-1) & 1 )
	{
		Sci(SCI_MARKERDELETE, nLine-1, 0);
		return FALSE;
	}
	else

	Sci(SCI_MARKERADD, nLine-1, 0);
	return TRUE;
}

BOOL CLuaEditor::Load(CFile* pFile)
{
	bool bReadOnly = GetReadOnly();
	SetReadOnly(false);

	const int blockSize = 131072;

	Sci(SCI_CLEARALL);

	char data[blockSize];
	size_t lenFile = pFile->Read(data, sizeof(data));
	while (lenFile > 0) 
	{
		Sci(SCI_ADDTEXT, lenFile, (long)data);
		lenFile = pFile->Read(data, sizeof(data));
	}

	Sci(SCI_SETEOLMODE, SC_EOL_CRLF);
	Sci(SCI_EMPTYUNDOBUFFER);
	Sci(SCI_SETUNDOCOLLECTION, 1);
	Sci(SCI_SETSAVEPOINT);
	Sci(SCI_GOTOPOS, 0);

	SetReadOnly(bReadOnly);
	return TRUE;
}

BOOL CLuaEditor::Save(CFile* pFile)
{
	const int blockSize = 131072;

	TextRange tr;
	char data[blockSize + 1];
	int lengthDoc = Sci(SCI_GETLENGTH);
	for (int i = 0; i < lengthDoc; i += blockSize) 
	{
		int grabSize = lengthDoc - i;
		if (grabSize > blockSize)
			grabSize = blockSize;

		tr.chrg.cpMin = i;
		tr.chrg.cpMax = i + grabSize;
		tr.lpstrText = data;
		Sci(SCI_GETTEXTRANGE, 0, long(&tr));
	
		pFile->Write(data, grabSize);
	}

	Sci(SCI_SETSAVEPOINT);

	return TRUE;	  
}

void CLuaEditor::ClearAllBreakpoints()
{
	Sci(SCI_MARKERDELETEALL, 0);
}

void CLuaEditor::SetBreakpoint(int nLine)
{
	Sci(SCI_MARKERADD, nLine-1, 0);
}

void CLuaEditor::SetBookMark(int nLine)
{

	if ( Sci(SCI_MARKERGET, nLine-1) & 4 )
	{
		Sci(SCI_MARKERDELETE, nLine-1, 2);
	}
	else
		Sci(SCI_MARKERADD, nLine-1, 2);

}

void CLuaEditor::GotoNextBookMark(int nLine, bool b)
{
	int line = Sci(SCI_MARKERNEXT,nLine,4);
	if (line == -1 && b)
		GotoNextBookMark(0,false);
	if(line!=-1)
		Sci(SCI_GOTOLINE, line);
}


void CLuaEditor::GotoLine(int nLine)
{
	Sci(SCI_GOTOLINE, nLine-1);

	Sci(SCI_MARKERDELETEALL, 1);
	Sci(SCI_MARKERADD, nLine-1, 1);
}

void CLuaEditor::SetBreakPointsIn(CProjectFile *pPF)
{
	pPF->RemoveAllBreakPoints();

	int nLine = Sci(SCI_MARKERNEXT, 0, 1);
	while ( nLine>=0 )
	{
		pPF->AddBreakPoint(nLine+1); // from 0-based to 1-based 
		nLine = Sci(SCI_MARKERNEXT, nLine+1, 1);
	}	
}

BOOL CLuaEditor::CanUndo()
{
	return Sci(SCI_CANUNDO);
}

void CLuaEditor::Undo()
{
	Sci(SCI_UNDO);
}

BOOL CLuaEditor::CanRedo()
{
	return Sci(SCI_CANREDO);
}

void CLuaEditor::Redo()
{
	Sci(SCI_REDO);
}

void CLuaEditor::SelectAll()
{
	Sci(SCI_SELECTALL);
}
int CLuaEditor::GetCurrentPos()
{
	return Sci(SCI_GETCURRENTPOS);
}
void CLuaEditor::SetCurrentPos(int pos)
{
	Sci(SCI_GOTOPOS, pos);
	int lc= Sci(SCI_LINESONSCREEN);
	if(pos<=lc)
		return;

	int ln = lc/2;
	Sci(SCI_LINESCROLL, 0, ln);
}

BOOL CLuaEditor::CanCutOrClear()
{
	int currentPos = GetCurrentPos();
	int anchor = Sci(SCI_GETANCHOR);

	return currentPos != anchor;
}

void CLuaEditor::Cut()
{
	Sci(SCI_CUT);
}

void CLuaEditor::Clear()
{
	Sci(SCI_CLEAR);
}

BOOL CLuaEditor::CanPaste()
{
	return Sci(SCI_CANPASTE);
}

void CLuaEditor::Paste()
{
	Sci(SCI_PASTE);
}

void CLuaEditor::Copy()
{
	Sci(SCI_COPY);
}

void CLuaEditor::GrabFocus()
{
	Sci(SCI_GRABFOCUS);
}

void CLuaEditor::SetEditorMargins()
{
	Sci(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
	int pixelWidth = 6 * Sci(SCI_TEXTWIDTH, STYLE_LINENUMBER, (int)"9");
	Sci(SCI_SETMARGINWIDTHN, 0, pixelWidth);

	Sci(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
	Sci(SCI_SETMARGINWIDTHN, 1, 15);
	Sci(SCI_SETMARGINSENSITIVEN, 1, TRUE);

	Sci(SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
	Sci(SCI_MARKERSETBACK, 0, RGB(0xff, 0x00, 0x00));
	Sci(SCI_MARKERSETFORE, 0, RGB(0x00, 0x00, 0x00));

	Sci(SCI_MARKERDEFINE, 1, SC_MARK_ARROW);

	Sci(SCI_MARKERDEFINE,  2, SC_MARK_ROUNDRECT);
	Sci(SCI_MARKERSETBACK, 2, RGB(0x80, 0xff, 0xff));
	Sci(SCI_MARKERSETFORE, 2, RGB(0x00, 0x00, 0x00));
	Sci(SCI_CALLTIPSETFORE, RGB(0x00, 0x00, 0x00));
	Sci(SCI_CALLTIPSETBACK, RGB(0xe8, 0xf7, 0xc4));
}

void CLuaEditor::SetCallStackMargins()
{
	Sci(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
	Sci(SCI_SETMARGINWIDTHN, 1, 10);
	Sci(SCI_SETMARGINSENSITIVEN, 1, FALSE);

	Sci(SCI_MARKERDEFINE, 0, SC_MARK_ARROW);
}

bool CLuaEditor::GetReadOnly()
{
	return	!!Sci(SCI_GETREADONLY);
}

void CLuaEditor::SetReadOnly(BOOL bReadOnly)
{
	Sci(SCI_SETREADONLY, bReadOnly);
}

void CLuaEditor::AddText(const char* szText)
{
	Sci(SCI_ADDTEXT, strlen(szText), (int)szText);
}

void CLuaEditor::ClearAll()
{
	Sci(SCI_CLEARALL);
}

CString CLuaEditor::GetLine(int nLine)
{
	CString strLine;
	int nLineLength = Sci(SCI_LINELENGTH, nLine-1);
	char s[2048];s[0]=0;
	if ( nLineLength>0 )
	{
		Sci(SCI_GETLINE, nLine-1, (int)s/*(int)strLine.GetBuffer(nLineLength)*/);
//		strLine.ReleaseBuffer();
		s[nLineLength]=0;
	}
	
	strLine = s;
	return strLine;
}

void CLuaEditor::FindText(CString str)
{
	char s[]="module";
	Sci(SCI_FINDTEXT, (int)s);
}

void CLuaEditor::GotoLastLine()
{
	int nLine = Sci(SCI_GETLINECOUNT);
	Sci(SCI_GOTOLINE, nLine);
}

int CLuaEditor::GetCurrentLine()
{
	return Sci(SCI_LINEFROMPOSITION, GetCurrentPos()) + 1;

}

void CLuaEditor::SetStackTraceLevel(int nLevel)
{
	Sci(SCI_MARKERDELETEALL, 0);
	Sci(SCI_MARKERADD, nLevel, 0);
}

CharacterRange CLuaEditor::GetSelection() 
{
	CharacterRange crange;
	crange.cpMin = Sci(SCI_GETSELECTIONSTART);
	crange.cpMax = Sci(SCI_GETSELECTIONEND);
	return crange;
}

BOOL CLuaEditor::PreparePrint(CDC *pDC, CPrintInfo *pInfo)
{
	CharacterRange crange = GetSelection();
	int startPos = crange.cpMin;
	int endPos = crange.cpMax;

	LONG lengthDoc = Sci(SCI_GETLENGTH);
	LONG lengthPrinted = 0;
	LONG lengthDocMax = lengthDoc;

	// Requested to print selection
	if (pInfo->m_pPD->m_pd.Flags & PD_SELECTION) {
		if (startPos > endPos) {
			lengthPrinted = endPos;
			lengthDoc = startPos;
		} else {
			lengthPrinted = startPos;
			lengthDoc = endPos;
		}

		if (lengthPrinted < 0)
			lengthPrinted = 0;
		if (lengthDoc > lengthDocMax)
			lengthDoc = lengthDocMax;
	}

	Sci(SCI_SETWRAPMODE, SC_WRAP_WORD);

	m_pages.RemoveAll();

	RangeToFormat frPrint;
	frPrint.hdc = pDC->GetSafeHdc();
	frPrint.hdcTarget = pDC->m_hAttribDC;
	frPrint.rcPage.left		= frPrint.rc.left	= 0;
	frPrint.rcPage.right	= frPrint.rc.right	= pDC->GetDeviceCaps(HORZRES);
	frPrint.rcPage.top		= frPrint.rc.top	= 0;
	frPrint.rcPage.bottom	= frPrint.rc.bottom = pDC->GetDeviceCaps(VERTRES);

	while (lengthPrinted < lengthDoc) {
		frPrint.chrg.cpMin = lengthPrinted;
		frPrint.chrg.cpMax = lengthDoc;

		m_pages.Add(lengthPrinted);

		lengthPrinted = Sci(SCI_FORMATRANGE, FALSE,
		                           reinterpret_cast<LPARAM>(&frPrint));
	}

	Sci(SCI_FORMATRANGE, FALSE, 0);

	pInfo->SetMaxPage(m_pages.GetSize());

	return TRUE;
}

void CLuaEditor::PrintPage(CDC* pDC, CPrintInfo* pInfo)
{
	RangeToFormat frPrint;
	frPrint.hdc = pDC->GetSafeHdc();
	frPrint.hdcTarget = pDC->m_hAttribDC;
	frPrint.rc.left = pInfo->m_rectDraw.left;
	frPrint.rc.right = pInfo->m_rectDraw.right;
	frPrint.rc.top = pInfo->m_rectDraw.top;
	frPrint.rc.bottom = pInfo->m_rectDraw.bottom;
	frPrint.rcPage.left = pInfo->m_rectDraw.left;
	frPrint.rcPage.right = pInfo->m_rectDraw.right;
	frPrint.rcPage.top = pInfo->m_rectDraw.top;
	frPrint.rcPage.bottom = pInfo->m_rectDraw.bottom;

	frPrint.chrg.cpMin = m_pages[pInfo->m_nCurPage - 1];
	frPrint.chrg.cpMax = Sci(SCI_GETLENGTH);

	Sci(SCI_FORMATRANGE, TRUE, reinterpret_cast<LPARAM>(&frPrint));
}

void CLuaEditor::EndPrint(CDC *pDC, CPrintInfo *pInfo)
{
	Sci(SCI_SETWRAPMODE, SC_WRAP_NONE);
}


void CLuaEditor::SetLuaLexer()
{
   const char font[] = "Verdana";
   const char monospace[] = "Courier";
   const short fontsize = 10;
   const char keywords[] = "module class self __finalize __init and break do else elseif end false for function global if in local nil not or repeat return then true until while";

   // set style bits, choose the right lexer (Lua) and set the keywords list
   Sci(SCI_SETSTYLEBITS,5,0);
   Sci(SCI_SETLEXER,SCLEX_LUA,0);
   Sci(SCI_SETKEYWORDS,0,(LPARAM)keywords);
   
   // set up basic features (iguides on, tab=3, tabs-to-spaces, EOL=CRLF)
   Sci(SCI_SETINDENTATIONGUIDES,1,0);
   Sci(SCI_SETTABWIDTH,4,0);
   Sci(SCI_SETUSETABS,0,0);
   Sci(SCI_SETEOLMODE,SC_EOL_CRLF,0);

   // now set up the styles (remember you have to set up font name for each style;
   // if you fail to do so, bold/italics will not work (only color will work)
   // !!colors are in format BGR!!

   // style 32: default
   Sci(SCI_STYLESETFONT,32, (LPARAM) font);
   Sci(SCI_STYLESETSIZE,32, fontsize);
   // style 0: whitespace
   Sci(SCI_STYLESETFORE,0, 0x808080);
   // style 1: comment (not used in Lua)
   // style 2: line comment (green)
   Sci(SCI_STYLESETFONT,2, (int)monospace);
   Sci(SCI_STYLESETSIZE,2, fontsize);
   Sci(SCI_STYLESETFORE,2, 0x00AA00);
   // style 3: doc comment (grey???)
   Sci(SCI_STYLESETFORE,3, 0x7F7F7F);      
   // style 4: numbers (blue)
   Sci(SCI_STYLESETFORE,4, 0x0000FF);
   // style 5: keywords (black bold)
   Sci(SCI_STYLESETFONT,5, (int)font);
   Sci(SCI_STYLESETSIZE,5, (int)fontsize);
   Sci(SCI_STYLESETFORE,5, 0x000000);
   Sci(SCI_STYLESETBOLD,5, 1);
   // style 6: double qouted strings (???)
   Sci(SCI_STYLESETFORE,6, 0xFF0000);
//   Sci(SCI_STYLESETFORE,6, 0x7F007F);
   // style 7: single quoted strings (???)
   Sci(SCI_STYLESETFORE,7, 0xFF0000);
//   Sci(SCI_STYLESETFORE,7, 0x7F007F);
   // style 8: UUIDs (IDL only, not used in Lua)
   // style 9: preprocessor directives (not used in Lua 4)
   // style 10: operators (black bold)
   Sci(SCI_STYLESETFONT,10, (int)font);
   Sci(SCI_STYLESETSIZE,10, fontsize);
   Sci(SCI_STYLESETFORE,10, 0x000000);
   Sci(SCI_STYLESETBOLD,10, 1);
   // style 11: identifiers (leave to default)
   Sci(SCI_STYLESETFONT,11, (int)font);
   Sci(SCI_STYLESETSIZE,11, fontsize);
   Sci(SCI_STYLESETFORE,11, 0x00000);
   Sci(SCI_STYLESETBOLD,11, 0);

   // style 12: end of line where string is not closed (black on violet, eol-filled)
   Sci(SCI_STYLESETFORE,12, 0x000000);
   Sci(SCI_STYLESETBACK,12, 0xE0C0E0);
   Sci(SCI_STYLESETEOLFILLED,12, 1);
}


void CLuaEditor::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!(nFlags&MK_CONTROL))
		{CWnd::OnMouseMove(nFlags, point);return;};

	if ( m_bShowCalltips && g_mainFrame->GetMode()==CMainFrame::modeDebugBreak )
	{
		CharacterRange cr = GetSelection();
		if(cr.cpMax==cr.cpMin)
			{CWnd::OnMouseMove(nFlags, point);return;};

		int  pos  =  Sci(SCI_POSITIONFROMPOINT, point.x, point.y);

		if( !((pos<=cr.cpMax)&&(pos>=cr.cpMin)) )
			{CWnd::OnMouseMove(nFlags, point);return;};
		
		CString sel_callTip = GetSelText();
		
		if(m_currCallTip == sel_callTip){CWnd::OnMouseMove(nFlags, point);return;};
			
		
		m_currCallTip = sel_callTip;

		g_mainFrame->GetCalltip(m_currCallTip.GetBuffer() );
		//g_calltip = "HEllo World";
		if( g_calltip.GetLength()/* && m_strCallTip!=g_calltip*/ )
		{
			if  (Sci(SCI_CALLTIPACTIVE) && m_strCallTip!=g_calltip)
					Sci(SCI_CALLTIPCANCEL);

			if (!Sci(SCI_CALLTIPACTIVE))
			{
				Sci(SCI_CALLTIPSHOW,  pos,  (int)g_calltip.GetBuffer());
				m_strCallTip = g_calltip;
			};
		}else if (Sci(SCI_CALLTIPACTIVE))
					Sci(SCI_CALLTIPCANCEL);
	}
	else if (Sci(SCI_CALLTIPACTIVE))
				Sci(SCI_CALLTIPCANCEL);

	CWnd::OnMouseMove(nFlags, point);
}

void CLuaEditor::SetLexer(int lexer, BOOL bDirect)
{
  Sci(SCI_SETLEXER, (WPARAM) lexer, 0/*, bDirect*/);
}
void CLuaEditor::SetKeyWords(int keywordSet, const char* keyWords, BOOL bDirect)
{
  Sci(SCI_SETKEYWORDS, (WPARAM) keywordSet, (LPARAM) keyWords/*, bDirect*/);
}

void CLuaEditor::StyleClearAll(BOOL bDirect)
{
  Sci(SCI_STYLECLEARALL, 0, 0/*, bDirect*/);
}

void CLuaEditor::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face) 
{
	StyleSetFore(style, fore);
	StyleSetBack(style, back);
	if (size >= 1)
		StyleSetSize(style, size);
	if (face) 
		StyleSetFont(style, face);
}

void CLuaEditor::StyleSetFore(int style, COLORREF fore, BOOL bDirect)
{
  Sci(SCI_STYLESETFORE, (WPARAM) style, (LPARAM) fore/*, bDirect*/);
  
}

void CLuaEditor::StyleSetBack(int style, COLORREF back, BOOL bDirect)
{
  Sci(SCI_STYLESETBACK, (WPARAM) style, (LPARAM) back/*, bDirect*/);
}

void CLuaEditor::StyleSetBold(int style, BOOL bold, BOOL bDirect)
{
  Sci(SCI_STYLESETBOLD, (WPARAM) style, (LPARAM) bold/*, bDirect*/);
}

void CLuaEditor::StyleSetSize(int style, int sizePoints, BOOL bDirect)
{
  Sci(SCI_STYLESETSIZE, (WPARAM) style, (LPARAM) sizePoints/*, bDirect*/);
}

void CLuaEditor::StyleSetFont(int style, const char* fontName, BOOL bDirect)
{
  Sci(SCI_STYLESETFONT, (WPARAM) style, (LPARAM) fontName/*, bDirect*/);
}

void CLuaEditor::setupLexer_ex()
{
  const char keywords[] = "super module class __finalize __init and break do else elseif end false for function global if in local nil not or repeat return then true until while";
  const char keywords2[] = "self";

  SetLexer(SCLEX_LUA);
  SetKeyWords(0, keywords);
  SetKeyWords(1, keywords2);


  //Setup styles
  SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Courier New"); //Verdana
  StyleClearAll();
  SetAStyle(SCE_LUA_DEFAULT, RGB(0, 0, 0));
  SetAStyle(SCE_LUA_COMMENT, RGB(0, 0x80, 0x80));////
  SetAStyle(SCE_LUA_COMMENTLINE, RGB(0, 0x80, 0));
  SetAStyle(SCE_LUA_COMMENTDOC, RGB(0, 0x80, 0));
  SetAStyle(SCE_C_COMMENTLINEDOC, RGB(0, 0x80, 0));
  SetAStyle(SCE_C_COMMENTDOCKEYWORD, RGB(0, 0x80, 0));
  SetAStyle(SCE_C_COMMENTDOCKEYWORDERROR, RGB(0, 0x80, 0));
  SetAStyle(SCE_LUA_NUMBER, RGB(0, 0x80, 0x80));
  SetAStyle(SCE_LUA_WORD, RGB(0, 0, 0xff));
  SetAStyle(SCE_LUA_WORD2, RGB(0xd0, 0, 0));
  SetAStyle(SCE_LUA_WORD7, RGB(0x80, 0x80, 0x80)); //!!!variable name
  SetAStyle(SCE_LUA_WORD8, RGB(0x80, 0, 0)); //!!!function name
  StyleSetBold(SCE_LUA_WORD, 0);
  SetAStyle(SCE_LUA_STRING, RGB(0x80, 0, 0x80));
  SetAStyle(SCE_LUA_IDENTIFIER, RGB(0, 0, 0));
  SetAStyle(SCE_LUA_PREPROCESSOR, RGB(0x80, 0, 0));
  SetAStyle(SCE_LUA_OPERATOR, RGB(0x80, 0x80, 0));
   Sci(SCI_SETTABWIDTH,4,0);
   Sci(SCI_SETUSETABS,1,0);
   Sci(SCI_SETTABINDENTS,7,0);
   Sci(SCI_SETINDENT,4,0);

   Sci(SCI_AUTOCSETSEPARATOR, 30);
}

void	CLuaEditor::AdjustCurrTabIndent()
{
	int pos			=   GetCurrentPos();
	
	int line		= 	Sci(SCI_LINEFROMPOSITION,pos);
	int prev_indent	=	Sci(SCI_GETLINEINDENTATION,line-1);
	Sci(SCI_SETLINEINDENTATION,line,prev_indent);
	int l_lenght	=	Sci(SCI_GETLINEINDENTPOSITION,line);
	Sci(SCI_SETCURRENTPOS,l_lenght);
	Sci(SCI_SETSELECTIONSTART,l_lenght);
	Sci(SCI_SETSELECTIONEND,l_lenght);
}
