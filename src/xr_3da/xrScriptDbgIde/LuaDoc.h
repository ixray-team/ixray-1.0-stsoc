// LuaDoc.h : interface of the CLuaDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUADOC_H__60B9BAB0_26C5_4E6D_AB07_404295AEF7E6__INCLUDED_)
#define AFX_LUADOC_H__60B9BAB0_26C5_4E6D_AB07_404295AEF7E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLuaView;
class CProjectFile;

class CLuaDoc : public CDocument
{
protected: // create from serialization only
	CLuaDoc();
	DECLARE_DYNCREATE(CLuaDoc)

// Attributes
public:

// Operations
public:
	CLuaView* GetView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLuaDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL DoFileSave() { return CDocument::DoFileSave(); };
	CProjectFile* GetProjectFile();
	BOOL IsInProject();
	virtual ~CLuaDoc();
	virtual void SetTitle(LPCTSTR lpszTitle);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLuaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUADOC_H__60B9BAB0_26C5_4E6D_AB07_404295AEF7E6__INCLUDED_)
