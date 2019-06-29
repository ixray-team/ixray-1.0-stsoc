#if !defined(AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_)
#define AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewFiles.h : header file
//

#include "projectFile.h"

class CTreeViewFiles : public CTreeView
{
protected:
	CTreeViewFiles();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewFiles)

// Attributes
public:

// Operations
public:
	void RemoveAll();
	HTREEITEM AddProjectFile(CString strName, CProjectFile * pf, HTREEITEM parent=NULL);
	void AddRoot(CString strProject);
	void AddFolder(CString& strName,HTREEITEM parent=NULL);
	void ExpandFiles();
	bool IsFolder(HTREEITEM itm);
	bool IsFile(HTREEITEM itm);
	bool IsRoot(HTREEITEM itm);
	BOOL Load(CArchive &ar);
	BOOL Save(CArchive &ar);
	BOOL Save(HTREEITEM itm, CArchive &ar);
	BOOL Load(HTREEITEM itm, CArchive &ar);
	HTREEITEM FindFolder(CString& name, HTREEITEM from = NULL);
	HTREEITEM FindFile(CString& name, HTREEITEM from = NULL);

	void ChangeSSstatus(CProjectFile* pf, EVSSStatus st);
protected:
	CTreeCtrl* m_pTree;
	HTREEITEM m_hRoot, m_hFilesFolder;
	CImageList m_images;
//	CString working_folder;
//drag-n-drop
protected:	
   CImageList*	m_pDragImage;	//содержит список изображений используемый  во  время переноса
   BOOL			m_bLDragging;
   HTREEITEM	m_hitemDrag,m_hitemDrop;
	HTREEITEM	CopyBranch( HTREEITEM htiBranch, HTREEITEM htiNewParent, 
                                                HTREEITEM htiAfter = TVI_LAST );
	HTREEITEM CopyItem( HTREEITEM hItem, HTREEITEM htiNewParent, 
									HTREEITEM htiAfter = TVI_LAST );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewFiles)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnProjectAddFiles();
	virtual void OnRunApplication();
	virtual void OnDebuggingOptions();
	virtual void OnAddFolder();
	virtual void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeViewFiles();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewFiles)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnVSSCheckIn();
	afx_msg void OnVSSCheckOut();
	afx_msg void OnVSSUndoCheckOut();
	afx_msg void OnVSSDifference();
	afx_msg void OnVSSGetLatest();
	afx_msg void OnVSSAddFile();
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
/*	long VSSGetStatus(HTREEITEM);
	void VSSCheckIn(HTREEITEM);
	void VSSCheckOut(HTREEITEM);
	void VSSUndoCheckOut(HTREEITEM);
	void VSSDifferences(HTREEITEM);
	void VSSGetLatest(HTREEITEM itm);
	void Reload(HTREEITEM itm);
	void Save(HTREEITEM itm);
*/
public:
	void VSSUpdateStatus(HTREEITEM itm, EVSSStatus st=vss_unknown);
	EVSSStatus GetItemStatus(HTREEITEM itm);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_)
