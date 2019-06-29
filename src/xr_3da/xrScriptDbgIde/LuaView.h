// LuaView.h : interface of the CLuaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUAVIEW_H__F7E3BD0D_1558_484D_9634_B138CC9D9B31__INCLUDED_)
#define AFX_LUAVIEW_H__F7E3BD0D_1558_484D_9634_B138CC9D9B31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LuaEditor.h"
#include "ScintillaCtrl.h"
#include "FindReplaceDlg.h"

class CLuaDoc;
class CLuaView : public CView
{
	CProjectFile* m_projFile;

protected: // create from serialization only
	CLuaView();
	DECLARE_DYNCREATE(CLuaView)
	virtual void OnActivateFrame(UINT nState, CFrameWnd* pFrameWnd);

// Attributes
public:
	CLuaDoc* GetDocument();
	CLuaDoc* GetDoc(){return (CLuaDoc*)m_pDocument;};
	CLuaEditor* GetEditor() { return &m_editor; };
	bool	_save();
	CProjectFile*	GetProjectFile(){return m_projFile;}
	void			SetProjectFile(CProjectFile* p){m_projFile = p;}
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLuaView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void OnActivateView(  BOOL bActivate,  CView* pActivateView,  CView* pDeactiveView );
	void ToggleBreakPoint(int nLine);
	void CloseFrame();
	void Activate();
	virtual ~CLuaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CScintillaCtrl& GetCtrl();
protected:
	CScintillaCtrl            m_Edit;               //The scintilla control
	CLuaEditor m_editor;
	int OnSci(SCNotification* pNotify);
//member for search?replace
	CString                   m_strFind;            //last find string
	CString                   m_strReplace;         //last replace string
	BOOL                      m_bCase;              //TRUE==case sensitive, FALSE==not
	BOOL                      m_bWord;              //TRUE==match whole word, FALSE==not
	BOOL                      m_bRegularExpression; //TRUE== regular expression, FALSE==not
	BOOL                      m_bFirstSearch;       //Is this the first search
	int                       m_bNext;              //TRUE==search down, FALSE== search up
	CScintillaFindReplaceDlg* m_pFindReplaceDlg;    //find or replace dialog
	BOOL                      m_bFindOnly;          //Is pFindReplace the find or replace?


//Search and replace support
	virtual void OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	virtual void TextNotFound(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
  virtual BOOL FindText(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	virtual void OnEditFindReplace(BOOL bFindOnly);
	virtual BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	virtual void OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase,	BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace);
	virtual void OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	virtual BOOL SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL bWord, BOOL m_bRegularExpression);
	virtual long FindAndSelect(DWORD dwFlags, TextToFind& ft);
  virtual void AdjustFindDialogPosition();
  virtual CScintillaFindReplaceDlg* CreateFindReplaceDialog();
	void OnEditFindNext();

//
	void createFunctionList(CMenu& mnu);
// Generated message map functions
protected:
	//{{AFX_MSG(CLuaView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnToggleBreakpoint();
	afx_msg void OnUpdateGotoLineNumber(CCmdUI* pCmdUI);
	afx_msg void OnGotoLineNumber();
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateFind(CCmdUI* pCmdUI);
	afx_msg void OnFunctionList();
	afx_msg void OnBreakPointList();
	afx_msg void OnCompleteWord();
	afx_msg void OnNextBookMark();
	afx_msg void OnToggleBookMark();
	afx_msg void OnAddWatch();
//}}AFX_MSG
	afx_msg LRESULT OnFindReplaceCmd(WPARAM, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LuaView.cpp
inline CLuaDoc* CLuaView::GetDocument()
   { return (CLuaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUAVIEW_H__F7E3BD0D_1558_484D_9634_B138CC9D9B31__INCLUDED_)
