#pragma once
struct Variable;

class CTreeViewLocals : public CTreeView
{
protected:
	virtual ~CTreeViewLocals();
	CTreeViewLocals();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewLocals)

	HTREEITEM FindParentItem(HTREEITEM start_from, char* name);
	void InsertRoot();
	CString GetItemName(HTREEITEM itm);
	CString GetItemFullName(HTREEITEM itm);

// Operations
public:
	void RemoveAll();
	void AddVariable(Variable *);
	void AddVariable(HTREEITEM ,Variable *);

protected:
	CTreeCtrl* m_pTree;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewFiles)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
