#if !defined(AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_)
#define AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LuaEditor.h : header file
//

#include "Platform.h"
#include "scintilla.h"

/////////////////////////////////////////////////////////////////////////////
// CLuaEditor window

class CProjectFile;
class CLuaView;

enum lists_types{
	eWordList=1,
	eFuncList,
	eBreakPointList
};

struct word_list_struct
{
	int				line_num;
	int				pos;
	CProjectFile*	pf;
};

struct func_list_struct
{
	func_list_struct():line_num(0),pf(0){}
	int				line_num;
	CProjectFile*	pf;
};

struct breakpoint_list_struct
{
	breakpoint_list_struct():line_num(0),pf(0){}
	int				line_num;
	CProjectFile*	pf;
};
typedef CTypedPtrArray<CPtrArray, word_list_struct*> CWordListArray;
typedef CTypedPtrArray<CPtrArray, func_list_struct*> CFuncListArray;
typedef CTypedPtrArray<CPtrArray, breakpoint_list_struct*> CBreakpointListArray;

extern CWordListArray			g_wordArray;
extern CFuncListArray			g_funcArray;
extern CBreakpointListArray		g_breakpointsArray;

template<class T>
void free(T& arr)
{	for(int i=0; i<arr.GetSize();++i) delete(arr[i]);
	arr.RemoveAll();}

class CLuaEditor : public CWnd
{
// Construction
public:
	CLuaEditor();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLuaEditor)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetLuaLexer();
	BOOL PreparePrint(CDC* pDC, CPrintInfo* pInfo);
	void PrintPage(CDC* pDC, CPrintInfo* pInfo);
	void EndPrint(CDC* pDC, CPrintInfo* pInfo);
	void SetStackTraceLevel(int nLevel);
	void SetCallStackMargins();
	int GetCurrentPos();
	void SetCurrentPos(int pos);
	int GetCurrentLine();
	void GotoLastLine();
	CString GetLine(int nLine);
	void ClearAll();
	void AddText(const char* szText);
	void SetReadOnly(BOOL bReadOnly);
	bool GetReadOnly();
	void SetEditorMargins();
	void GrabFocus();
	void Copy();
	void Paste();
	BOOL CanPaste();
	void Clear();
	void Cut();
	BOOL CanCutOrClear();
	void SelectAll();
	void Redo();
	BOOL CanRedo();
	void Undo();
	BOOL CanUndo();
	void SetBreakPointsIn(CProjectFile* pPF);
	void GotoLine(int nLine);
	void SetBreakpoint(int nLine);
	void ClearAllBreakpoints();
	BOOL Save(CFile* pFile);
	BOOL Load(CFile* pFile);
	BOOL ToggleBreakpoint(int nLine);
	int LineFromPoint(CPoint& pt);
	BOOL Create(CWnd* pParentWnd, UINT nCtrlID);
	virtual ~CLuaEditor();
	void SetBookMark(int nLine);
	void GotoNextBookMark(int nLine, bool b = true);
//find 
	CString GetSelText();
	long	GetSelectionStart(BOOL bDirect=TRUE);
	long	GetSelectionEnd(BOOL bDirect=TRUE);
	int		GetSelText(char* text, BOOL bDirect = TRUE);
	int		GetLength(BOOL bDirect = TRUE);
	void	HideSelection(BOOL normal, BOOL bDirect = TRUE);
	void	ReplaceSel(const char* text, BOOL bDirect = TRUE);
	long	FindText(int flags, TextToFind* ft, BOOL bDirect = TRUE);
	long	FindTextNext(int flags, TextToFind* ft, BOOL bDirect = TRUE);
	void	SetSel(long start, long end, BOOL bDirect = TRUE);
	int		PointXFromPosition(long pos, BOOL bDirect = TRUE);
	int		PointYFromPosition(long pos, BOOL bDirect = TRUE);
	void	CompleteWord(const char* word);
	void	createFunctionList(CMenu& mnu);
	bool	createWordList(CMenu& mnu);
	long	GetWordStart(long end);

//lexer
	void	SetLexer(int lexer, BOOL bDirect = TRUE);
	void	SetKeyWords(int keywordSet, const char* keyWords, BOOL bDirect = TRUE);
	void	StyleClearAll(BOOL bDirect = TRUE);
	void	SetAStyle(int style, COLORREF fore, COLORREF back=RGB(0xff, 0xff, 0xff), int size=-1, const char *face=0);
	void	StyleSetFore(int style, COLORREF fore, BOOL bDirect = TRUE);
	void	StyleSetBack(int style, COLORREF back, BOOL bDirect = TRUE);
	void	StyleSetBold(int style, BOOL bold, BOOL bDirect = TRUE);
	void	StyleSetSize(int style, int sizePoints, BOOL bDirect = TRUE);
	void	StyleSetFont(int style, const char* fontName, BOOL bDirect = TRUE);
	void	setupLexer_ex();

	void	AdjustCurrTabIndent();

	void Print(bool showDialog);
	CharacterRange GetSelection();
	void GotoLineDlg();
	void FindText(CString str);
	CLuaView*	m_lua_view;
	int Sci(int nCmd, int wParam=0, int lParam=0);

protected:
	int (*m_fnScintilla)(void*,int,int,int);
	void *m_ptrScintilla;
	CUIntArray	m_pages;
	BOOL m_bShowCalltips;
	CString m_strCallTip;
	CString m_currCallTip;


	// Generated message map functions
protected:
	//{{AFX_MSG(CLuaEditor)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_)
