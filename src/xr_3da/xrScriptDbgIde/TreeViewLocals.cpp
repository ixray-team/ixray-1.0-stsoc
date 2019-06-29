#include "stdafx.h"
#include "TreeViewLocals.h"

#include "ide2.h"

#include "MainFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"

IMPLEMENT_DYNCREATE(CTreeViewLocals, CTreeView)

CTreeViewLocals::CTreeViewLocals()
{}

CTreeViewLocals::~CTreeViewLocals()
{}

//ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
//ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
//ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
//ON_COMMAND(ID_PROJECT_ADD_FILES, OnProjectAddFiles)
//ON_COMMAND(ID_PROJECT_PROPERTIES, OnProjectProperties)

BEGIN_MESSAGE_MAP(CTreeViewLocals, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewLocals)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CTreeViewLocals::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTree = &GetTreeCtrl();

	InsertRoot();
	return 0;
}

void CTreeViewLocals::InsertRoot()
{
	TV_INSERTSTRUCT var;
	var.hParent = NULL;
	var.hInsertAfter = TVI_LAST;
	var.item.pszText = "\\";
	var.item.mask = TVIF_TEXT;

	m_pTree->InsertItem(&var);
}
void CTreeViewLocals::RemoveAll()
{
	m_pTree->DeleteAllItems();
	InsertRoot();
}

BOOL CTreeViewLocals::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;

	return CTreeView::PreCreateWindow(cs);
}

void CTreeViewLocals::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
/*	if ( pNMTreeView->itemNew.hItem == m_hFilesFolder )
	{
		if ( pNMTreeView->action==TVE_EXPAND )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 3, 3);
		else if ( pNMTreeView->action==TVE_COLLAPSE )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 2, 2);
	}
*/
	*pResult = 0;
}

CString CTreeViewLocals::GetItemName(HTREEITEM itm)
{
	CString s =m_pTree->GetItemText(itm);
	return s.Left(s.Find(" "));
}

CString CTreeViewLocals::GetItemFullName(HTREEITEM itm)
{
	CArray<CString,CString> names;
	while( itm!=m_pTree->GetRootItem() ){
		names.Add(GetItemName(itm));
		itm = m_pTree->GetParentItem(itm);
	};
	CString res;
	BOOL bFirst = TRUE;
	while( names.GetCount() ){
		if(!bFirst){
			res.Append(".");
		}else
			bFirst = FALSE;

		res.Append(names.GetAt(names.GetCount()-1));
		names.RemoveAt(names.GetCount()-1);
	}
	return res;
}

HTREEITEM CTreeViewLocals::FindParentItem(HTREEITEM start_from, char* name)
{
	HTREEITEM itm = m_pTree->GetChildItem(start_from);
	while ( itm != NULL ){
		CString varName =GetItemName(itm);
		if( varName.Compare(name)==0 )
			return itm;
		itm = m_pTree->GetNextSiblingItem(itm);
	}
	return NULL;
}

void CTreeViewLocals::AddVariable(Variable* var)
{

	HTREEITEM itm_root = m_pTree->GetRootItem();
	
	if(!itm_root){
		AddVariable(itm_root,var);
		return;
	}


	char name[255];
	strcpy(name,var->szName);	

	char seps[]   = ".";
	char *token;

	token = strtok( name, seps );
	while( token != NULL )
	{
		HTREEITEM itm = FindParentItem(itm_root, token);
		if(itm==NULL)
			break;

		itm_root = itm;
		token = strtok( NULL, seps );
	}
	
	strcpy(var->szName,token);
	AddVariable(itm_root,var);
	m_pTree->Expand(m_pTree->GetRootItem(),TVE_EXPAND );
}

void CTreeViewLocals::AddVariable(HTREEITEM parent, Variable* variable)
{
//var->szName, var->szType,var->szValue
//char* szName, char* szType, char* szValue
	
	char sText[512];
	sprintf(sText,"%s %s %s",variable->szName,variable->szType, variable->szValue);
	TV_INSERTSTRUCT var;
	var.hParent = parent;
	var.hInsertAfter = TVI_LAST;
	var.item.pszText = sText;
	var.item.mask = TVIF_TEXT;

	m_pTree->InsertItem(&var);
}

void CTreeViewLocals::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM hItem = m_pTree->HitTest(pt, &nFlags);
	
	if(m_pTree->GetItemText(hItem).Find(" table ") == -1)
		return;

	CString sVarName = GetItemFullName(hItem);
	
	g_mainFrame->OpenVarTable(sVarName.GetBuffer());

	*pResult = 0;
}
