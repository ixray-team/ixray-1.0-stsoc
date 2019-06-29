// TreeViewFiles.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "TreeViewFiles.h"

#include "MainFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "SSConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles
CString str_WhoCheckedOut(IVSSItemPtr vss_item);


IMPLEMENT_DYNCREATE(CTreeViewFiles, CTreeView)

CTreeViewFiles::CTreeViewFiles()
{
//	working_folder  = 	AfxGetApp()->GetProfileString("options","sSafeFolder", "" );
	m_hRoot			= NULL;
	m_hFilesFolder	= NULL;
	m_bLDragging	= FALSE;
	m_hitemDrag		= NULL;
	m_hitemDrop		= NULL;
}

CTreeViewFiles::~CTreeViewFiles()
{
}


BEGIN_MESSAGE_MAP(CTreeViewFiles, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewFiles)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT , OnBeginEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT , OnEndEdit)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_CLICK, OnLclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)

	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()

	//}}AFX_MSG_MAP
	ON_COMMAND(ID_PROJECT_ADD_FILES,  OnProjectAddFiles)
	ON_COMMAND(ID_PROJECT_RUNPROJECT, OnRunApplication)
	ON_COMMAND(ID_PROJECT_DEBUGGING,  OnDebuggingOptions)
	ON_COMMAND(ID_PROJECT_ADDFOLDER,  OnAddFolder)

	ON_COMMAND(ID_VSS_CHECKIN,			OnVSSCheckIn)
	ON_COMMAND(ID_VSS_CHECKOUT,			OnVSSCheckOut)
	ON_COMMAND(ID_VSS_UNDOCHECKOUT,		OnVSSUndoCheckOut)
	ON_COMMAND(ID_VSS_DIFFERENCE,		OnVSSDifference)
	ON_COMMAND(ID_VSS_GETLATESTVERSION,	OnVSSGetLatest)

	ON_COMMAND(ID_VSS_ADDTOSOURCECONTROL,	OnVSSAddFile)

	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles drawing
void CTreeViewFiles::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;
	if( IsRoot(m_hitemDrag) )
		return;

	m_pDragImage = m_pTree->CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() возвращает  NULL если нет списка изображаний
	// связанного с деревом
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}
void CTreeViewFiles::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM	hitem;
	UINT		flags;

	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);
		if ((hitem = m_pTree->HitTest(point, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);
			m_pTree->SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}
	
	CTreeView::OnMouseMove(nFlags, point);
}

void CTreeViewFiles::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonUp(nFlags, point);

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;

		// Удаляем подсветку цели
		m_pTree->SelectDropTarget(NULL);

		if( m_hitemDrag == m_hitemDrop || !IsFolder(m_hitemDrop) )
			return;

		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = m_pTree->GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}

		m_pTree->Expand( m_hitemDrop, TVE_EXPAND ) ;

		HTREEITEM htiNew = CopyBranch( m_hitemDrag, m_hitemDrop, TVI_LAST );

		m_pTree->DeleteItem(m_hitemDrag);
		m_pTree->SelectItem( htiNew );

		CProject* pProject = g_mainFrame->GetProject();
		pProject->SetModifiedFlag(TRUE);

	}

}


void CTreeViewFiles::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles diagnostics

#ifdef _DEBUG
void CTreeViewFiles::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeViewFiles::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles message handlers

int CTreeViewFiles::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTree = &GetTreeCtrl();

	m_images.Create (IDB_IL_FILE, 16, 1, RGB(0,255,0));
	int cc = m_images.GetImageCount();
	m_pTree->SetImageList (&m_images, TVSIL_NORMAL);
	

	return 0;
}

void CTreeViewFiles::RemoveAll()
{
	m_pTree->DeleteAllItems();
}

void CTreeViewFiles::AddRoot(CString strProject)
{
	CString strLabel;
	strLabel.Format("Project '%s'", strProject);

	TV_INSERTSTRUCT root;
	root.hParent = NULL;
	root.hInsertAfter = TVI_SORT;
	root.item.iImage = 1;
	root.item.iSelectedImage = 1;
	root.item.pszText = strLabel.GetBuffer(0);
	root.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hRoot = m_pTree->InsertItem(&root);

	TV_INSERTSTRUCT files;
	files.hParent = m_hRoot;
	files.hInsertAfter = TVI_SORT;
	files.item.iImage = 2;
	files.item.lParam = 0;
	files.item.iSelectedImage = 2;
	files.item.pszText = "Lua scripts";
	files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hFilesFolder = m_pTree->InsertItem(&files);

//	m_pTree->SetItemData(m_hFilesFolder,888);
}
void CTreeViewFiles::AddFolder(CString& strName, HTREEITEM parent)
{
	if(FindFolder(strName)) //tmp!!!
		return;

	HTREEITEM to = m_hRoot;
	HTREEITEM itm_ = m_pTree->GetSelectedItem();
	if( IsFolder(itm_) ) 
		to = itm_;
	if(parent)
		to = parent;

	TV_INSERTSTRUCT files;
	files.hParent = to;
	files.hInsertAfter = TVI_SORT;
	files.item.iImage = 2;
	files.item.iSelectedImage = 2;
	files.item.pszText = strName.GetBuffer(0);
	files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	HTREEITEM itm = m_pTree->InsertItem(&files);

	m_pTree->SetItemData(itm, 888);
}

HTREEITEM CTreeViewFiles::AddProjectFile(CString strName, CProjectFile * pf, HTREEITEM parent)
{
	HTREEITEM to = m_hFilesFolder;
	HTREEITEM itm_ = m_pTree->GetSelectedItem();
	if( IsFolder(itm_) ) 
		to = itm_;
	if(parent)
		to=parent;

	HTREEITEM itm = FindFile(strName);
	if(!itm){
	
	TV_INSERTSTRUCT file;
	file.hParent = to;
	file.hInsertAfter = TVI_LAST;
	file.item.iImage = 4;
	file.item.iSelectedImage = 4;
	file.item.pszText = strName.GetBuffer(0);
	file.item.lParam = (LPARAM)pf;
	file.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	itm = m_pTree->InsertItem(&file);

	}else{
		m_pTree->SetItemData(itm,(LPARAM)pf);
	}

	VSSUpdateStatus(itm);
	return itm;
}

BOOL CTreeViewFiles::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS;
	
	return CTreeView::PreCreateWindow(cs);
}

void CTreeViewFiles::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if ( IsFolder(pNMTreeView->itemNew.hItem) )
	{
		if ( pNMTreeView->action==TVE_EXPAND )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 3, 3);
		else if ( pNMTreeView->action==TVE_COLLAPSE )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 2, 2);
	}

	if ( IsFolder(pNMTreeView->itemNew.hItem)  
		  && pNMTreeView->action==TVE_EXPAND )
	{
			HTREEITEM child =  m_pTree->GetChildItem(pNMTreeView->itemNew.hItem);
						
			while(child){
				if( IsFile(child) )
					VSSUpdateStatus(child);

				child = m_pTree->GetNextSiblingItem(child);
			}
	}
	*pResult = 0;

}

void CTreeViewFiles::OnLclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM item = m_pTree->HitTest(pt, &nFlags);
	if( IsFile(item))
		VSSUpdateStatus(item);
}

void CTreeViewFiles::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM itm = m_pTree->HitTest(pt, &nFlags);
	
	if(!m_pTree->SelectItem(itm)) return;
//	long stat = VSSGetStatus(itm);
	EVSSStatus stat = GetItemStatus(itm);


	HMENU hMenu = NULL;
	if ( IsRoot(itm) || IsFolder(itm) ){
		hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_PROJECT_MENU));

		if ( !hMenu )
			return;

		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu) 
		{
			DestroyMenu(hMenu);
			return;
		}

		POINT mouse;
		GetCursorPos(&mouse);
		::SetForegroundWindow(m_hWnd);	
		::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, m_hWnd, NULL);

		DestroyMenu(hMenu);
	}

	if( IsFile(itm)){
		hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
		if ( !hMenu )
			return;

		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu) 
		{
			DestroyMenu(hMenu);
			return;
		}

		CMenu mnu;
		mnu.Attach(hSubMenu);
//		HTREEITEM itm = m_pTree->GetSelectedItem();
//		long stat = VSSGetStatus(itm);
		UINT mnu_itm;
		mnu_itm = mnu.GetMenuItemID(0);
		mnu.EnableMenuItem(mnu_itm,(stat==vss_checked_out_me)?MF_ENABLED : MF_GRAYED);


		mnu_itm = mnu.GetMenuItemID(1);
		mnu.EnableMenuItem(mnu_itm,((stat==vss_not_checked_out)||(stat==vss_checked_out))?MF_ENABLED : MF_GRAYED );
		mnu_itm = mnu.GetMenuItemID(2);
		mnu.EnableMenuItem(mnu_itm,(stat==vss_checked_out_me)?MF_ENABLED : MF_GRAYED);
		mnu_itm = mnu.GetMenuItemID(3);
		mnu.EnableMenuItem(mnu_itm,((stat != vss_no_ss)&&(stat != vss_unknown))?MF_ENABLED : MF_GRAYED);
		mnu_itm = mnu.GetMenuItemID(4);
		mnu.EnableMenuItem(mnu_itm,((stat != vss_no_ss)&&(stat != vss_unknown))?MF_ENABLED : MF_GRAYED);

		mnu_itm = mnu.GetMenuItemID(5);
		mnu.EnableMenuItem(mnu_itm,((stat == vss_no_ss)||(stat == vss_unknown))?MF_ENABLED : MF_GRAYED);

			POINT mouse;
			GetCursorPos(&mouse);
			::SetForegroundWindow(m_hWnd);	
	//		::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, m_hWnd, NULL);
			mnu.TrackPopupMenu(/*hSubMenu,*/ 0, mouse.x, mouse.y, this, NULL);
			
			DestroyMenu(hMenu);

	}
	*pResult = 0;
}

void CTreeViewFiles::OnProjectAddFiles() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->AddFiles();
}

void CTreeViewFiles::OnRunApplication() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->OnRunApplication();
}

void CTreeViewFiles::OnDebuggingOptions() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->OnDebugOptions();
}

void CTreeViewFiles::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM hItem = m_pTree->HitTest(pt, &nFlags);
	if(!hItem)
		return;

	if( IsFolder(hItem) )
		return;

	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	if ( pPF )
	{
		CLuaView* pView = theApp.OpenProjectFilesView(pPF);

/*		if(pView){
			pPF->SetLuaView(pView);
			pPF->Check_view();
			pView->Activate();
			VSSUpdateStatus(hItem);
		}*/
	}

	*pResult = 0;
}

void CTreeViewFiles::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_pTree->GetSelectedItem();

	if( hItem && pTVKeyDown->wVKey == VK_RETURN ){
		OnDblclk(pNMHDR, pResult);
		return;
	}

	if ( hItem && pTVKeyDown->wVKey == VK_DELETE )
	{
		CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
		if ( pPF )
		{
			if( m_pTree->GetChildItem(hItem) ){
				AfxMessageBox("Folder is not empty");
				return;
			}

			m_pTree->DeleteItem(hItem);
			CProject* pProject = g_mainFrame->GetProject();
			pProject->RemoveFile(pPF);
		}
	}
	if ( hItem && pTVKeyDown->wVKey == VK_F2 )
	{
		if(IsFolder(hItem))
			CEdit * edt = m_pTree->EditLabel(hItem);
		
	}

	*pResult = 0;
}

void CTreeViewFiles::ExpandFiles()
{
	GetTreeCtrl().Expand(m_hRoot, TVE_EXPAND );
}


CString str_WhoCheckedOut(IVSSItemPtr vss_item)
{
	CString str_user(_T("None"));
	IVSSCheckoutsPtr checkOuts;
	vss_item->get_Checkouts(&checkOuts);

	IVSSCheckoutPtr vss_checkOutItem;
	checkOuts->get_Item(_variant_t(1L), &vss_checkOutItem);
	CComBSTR str_userName;
	vss_checkOutItem->get_Username(&str_userName);

	long l_count;
	checkOuts->get_Count(&l_count);
	if (l_count == 1)
	{
		CW2A pszA( str_userName );
		str_user.Format(_T("%s"), pszA);
	}
	else if (l_count > 1)
	{
		CW2A pszA( str_userName );
		str_user.Format(_T("%s et.al."), pszA);
	}
	return str_user;
}

EVSSStatus CTreeViewFiles::GetItemStatus(HTREEITEM itm)
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return vss_unknown;

	if(!IsFile(itm))
		return vss_unknown;

	DWORD_PTR pp = m_pTree->GetItemData(itm);
	if(pp){
		CProjectFile* pf = (CProjectFile*)pp;
		 return pf->GetSS_status();
	}
	return vss_unknown;
}

void CTreeViewFiles::VSSUpdateStatus(HTREEITEM itm, EVSSStatus status/*=vss_unknown*/)
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	if(!itm)
		return;


	EVSSStatus st =status;
	if(st == vss_unknown)
		st = GetItemStatus(itm);

//	long stat = VSSGetStatus(itm);
		switch (st)
		{
		
//		case -1:{
		case vss_unknown:
		case vss_no_ss:{
			m_pTree->SetItemImage(itm, 4, 4);
/*			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if( pf->GetLuaView() )
					pf->GetLuaView()->GetEditor()->SetReadOnly(stat!=-1);
			}
*/
		}break;

		case vss_not_checked_out:{
			
			m_pTree->SetItemImage(itm, 5, 5);
/*			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if(pf->GetLuaView())
					pf->GetLuaView()->GetEditor()->SetReadOnly(stat!=-1);
			}
*/
			}break;
		case vss_checked_out:
			m_pTree->SetItemImage(itm, 5, 5);
			break;
		case vss_checked_out_me :{
			m_pTree->SetItemImage(itm, 6, 6);
/*			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if(pf->GetLuaView())
					pf->GetLuaView()->GetEditor()->SetReadOnly(FALSE);
			}
*/
			}break;
		default:
			break;
		}
}
bool CTreeViewFiles::IsFolder(HTREEITEM itm)
{
	return itm && ( itm==m_hFilesFolder || m_pTree->GetItemData(itm)==888 );
}

bool CTreeViewFiles::IsFile(HTREEITEM itm)
{
	return itm && !IsFolder(itm) && IsFolder( m_pTree->GetParentItem(itm) );
}

bool CTreeViewFiles::IsRoot(HTREEITEM itm)
{
	return itm && itm==m_hRoot;
}
/*
long CTreeViewFiles::VSSGetStatus(HTREEITEM itm)
{
	if( !IsFile(itm) )
		return -1;
	if(!theApp.m_ssConnection.b_IsConnected())
		return -1;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CString str_;
	str_.Format("%s%s",m_working_folder,str);
	CComBSTR file_name = str_;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	if(vssItem==NULL)
		return -1;
	long stat;
	HRESULT hr;
	CString str_temp;
	if((hr = vssItem->get_IsCheckedOut(&stat)) == S_OK){
		return stat;
	}
	return -1;
}*/

void CTreeViewFiles::OnVSSCheckIn(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_check_in();
	VSSUpdateStatus(hItem);

/*	HTREEITEM hItem = m_pTree->GetSelectedItem();

	Save(hItem);
	VSSCheckIn(hItem);
	VSSUpdateStatus(hItem);
*/
}

void CTreeViewFiles::OnVSSCheckOut(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_check_out();
	VSSUpdateStatus(hItem);
/*	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSCheckOut(hItem);
	VSSUpdateStatus(hItem);
*/
}

void CTreeViewFiles::OnVSSUndoCheckOut(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_undo_check_out();
	VSSUpdateStatus(hItem);

/*	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSUndoCheckOut(hItem);
	VSSUpdateStatus(hItem);
*/
}

void CTreeViewFiles::OnVSSGetLatest(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_get_latest();
	VSSUpdateStatus(hItem);
/*	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSGetLatest(hItem);
*/
}

void CTreeViewFiles::OnVSSDifference(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_difference();

/*	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSDifferences(hItem);
*/
}

void CTreeViewFiles::OnVSSAddFile(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	pPF->SS_add_to_ss();

}
/*
void CTreeViewFiles::VSSCheckIn(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;
	Save(itm);
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkin(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		VSSUpdateStatus(itm);
	}

}

void CTreeViewFiles::VSSCheckOut(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;

	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkout(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		VSSUpdateStatus(itm);
		Reload(itm);
	}

}

void CTreeViewFiles::VSSUndoCheckOut(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->UndoCheckout(bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		Reload(itm);
		VSSUpdateStatus(itm);
	}

}

void CTreeViewFiles::VSSGetLatest(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Get(&bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		Reload(itm);
		VSSUpdateStatus(itm);
	}

}

void CTreeViewFiles::VSSDifferences(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	VARIANT_BOOL res;
	vssItem->get_IsDifferent(bstr_localSpec, &res);
	if (b_DisplayAnyError())
		return;

	if(res==0){
		AfxMessageBox("The files are identical");
		return;
	}else{
		CComBSTR bstr_localSpec_ss = bstr_localSpec;//+".ssver";
		bstr_localSpec_ss.Append(".ssver");
		vssItem->Get(&bstr_localSpec_ss,0);
		if (b_DisplayAnyError())
			return;

		CString f1,f2;
		f1 = CW2A(bstr_localSpec);
		f2 = CW2A(bstr_localSpec_ss);
		CFileStatus fs;
		CFile::GetStatus(f2,fs);
		fs.m_attribute &= ~(CFile::readOnly);
		CFile::SetStatus(f2,fs);
		CString cmd = ((CIdeApp*)AfxGetApp())->m_comparerCmd;
		if(cmd.GetLength()==0){
			AfxMessageBox("File comparer executable not defined");
			return;
		}

		CString args;
		args.Format(((CIdeApp*)AfxGetApp())->m_comparerFormat, f1, f2);
		spawnl(_P_NOWAIT, cmd.GetBuffer(), args.GetBuffer());
	}

}

void  CTreeViewFiles::Save(HTREEITEM itm){
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(itm);
	if ( pPF )
	{
		if(pPF->GetLuaView())
			pPF->GetLuaView()->_save();
	}

}

void  CTreeViewFiles::Reload(HTREEITEM itm){

	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(itm);
	if ( pPF )
	{
		CLuaView* pView = theApp.OpenProjectFilesView(pPF);
		CFile fin;
		if ( fin.Open(pPF->GetPathName(), CFile::modeRead) )
			pView->GetEditor()->Load(&fin);
	}
}
*/

void CTreeViewFiles::OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
        

	if ( m_pTree->GetItemData(pTVDispInfo->item.hItem )== 888 ){

        *pResult = 0;
		return;
	}
        *pResult = TRUE;
}

void CTreeViewFiles::OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
        // Set result to TRUE to accept the changes
        *pResult = TRUE;
}
void CTreeViewFiles::OnAddFolder()
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->AddFolder();
}


HTREEITEM CTreeViewFiles::CopyBranch( HTREEITEM htiBranch, HTREEITEM htiNewParent, 
                                                HTREEITEM htiAfter /*= TVI_LAST*/ )
{
        HTREEITEM hChild;

        HTREEITEM hNewItem = CopyItem( htiBranch, htiNewParent, htiAfter );
        hChild = m_pTree->GetChildItem(htiBranch);
        while( hChild != NULL)
        {
                // рекурсивно переносим все элементы
                CopyBranch(hChild, hNewItem);  
                hChild = m_pTree->GetNextSiblingItem( hChild );
        }
        return hNewItem;
}

HTREEITEM CTreeViewFiles::CopyItem( HTREEITEM hItem, HTREEITEM htiNewParent, 
									HTREEITEM htiAfter /*= TVI_LAST*/ )
{
 TV_INSERTSTRUCT tvstruct;
 HTREEITEM hNewItem;
 CString sText;

 // берем информацию источника
 tvstruct.item.hItem = hItem;
 tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
 m_pTree->GetItem(&tvstruct.item);
 sText = m_pTree->GetItemText( hItem );

 tvstruct.item.cchTextMax = sText.GetLength();
 tvstruct.item.pszText = sText.LockBuffer();

 // Вставляем элемент в заданное место
 tvstruct.hParent = htiNewParent;
 tvstruct.hInsertAfter = htiAfter;
 tvstruct.item.mask = TVIF_IMAGE| TVIF_SELECTEDIMAGE | TVIF_TEXT;
 hNewItem = m_pTree->InsertItem(&tvstruct);sText.ReleaseBuffer();

 // Теперь копируем данные и состояние элемента
 m_pTree->SetItemData( hNewItem, m_pTree->GetItemData(hItem ));
 m_pTree->SetItemState( hNewItem, m_pTree->GetItemState(hItem, TVIS_STATEIMAGEMASK ),TVIS_STATEIMAGEMASK );

 // Вызываем виртуальную функцию для дальнейшей обработки наследованного класса
// OnItemCopied( hItem, hNewItem);
 return hNewItem;
}

BOOL CTreeViewFiles::Save(CArchive &ar)
{
	HTREEITEM itm = m_pTree->GetChildItem(m_hRoot);
	while(itm){
		if( IsFolder(itm) )
			Save(itm, ar);

		itm = m_pTree->GetNextSiblingItem(itm);
	}
	ar << CString("__end_of_treeView");
	
	return TRUE;
}

BOOL CTreeViewFiles::Load(CArchive &ar)
{
	CString parent_name;
	CString child_name;
	int isFolder;
	while(true){
		ar >> parent_name;
		if(parent_name.Compare("__end_of_treeView")==0 )
			return TRUE;
		ar >> child_name;
		ar >> isFolder;
		HTREEITEM parent_item = FindFolder(parent_name);
		if(!parent_item){
			CString msg;
			msg.Format("Error loading project file");
			AfxMessageBox(msg);
			return FALSE;
		}
		if(isFolder==1)
			AddFolder(child_name, parent_item);
		else
			AddProjectFile(child_name, NULL, parent_item);

	}

	return TRUE;
}

BOOL CTreeViewFiles::Save(HTREEITEM itm_, CArchive &ar)
{
	ar << m_pTree->GetItemText( m_pTree->GetParentItem(itm_) );
	ar << m_pTree->GetItemText(itm_);
	if(IsFolder(itm_))
		ar << 1;
	else
		ar << 0;

	HTREEITEM itm = m_pTree->GetChildItem(itm_);
	while(itm){
		if( IsFolder(itm)||IsFile(itm) )
			Save(itm, ar);

		itm = m_pTree->GetNextSiblingItem(itm);
	}
	return TRUE;
}

BOOL CTreeViewFiles::Load(HTREEITEM itm, CArchive &ar)
{
	return TRUE;
}

HTREEITEM CTreeViewFiles::FindFolder(CString& name, HTREEITEM itm_root)
{
	if(!itm_root)
		itm_root = m_hRoot;
	CString itm_name = m_pTree->GetItemText(itm_root);
	if( (IsFolder(itm_root)||IsRoot(itm_root)) && itm_name.CompareNoCase(name)==0)
		return itm_root;

	HTREEITEM itm = m_pTree->GetChildItem(itm_root);
	while(itm){
		HTREEITEM res = FindFolder(name,itm);
		if(res)
			return res;

		itm = m_pTree->GetNextSiblingItem(itm);
	}
	return NULL;
}

HTREEITEM CTreeViewFiles::FindFile(CString& name, HTREEITEM itm_root)
{
	if(!itm_root)
		itm_root = m_hRoot;
	CString itm_name = m_pTree->GetItemText(itm_root);
	if(IsFile(itm_root) && itm_name.Compare(name)==0)
		return itm_root;

	HTREEITEM itm = m_pTree->GetChildItem(itm_root);
	while(itm){
		HTREEITEM res = FindFile(name,itm);
		if(res)
			return res;

		itm = m_pTree->GetNextSiblingItem(itm);
	}
	return NULL;
}

void CTreeViewFiles::ChangeSSstatus(CProjectFile* pf, EVSSStatus st)
{
		HTREEITEM itm = m_pTree->GetFirstVisibleItem();
		if(itm&&m_pTree->GetItemData(itm)==(LPARAM)pf){
			VSSUpdateStatus(itm,st);
			return;
			}

		while (itm = m_pTree->GetNextVisibleItem(itm)){
			if(itm&&m_pTree->GetItemData(itm)==(LPARAM)pf){
				VSSUpdateStatus(itm,st);
				return;
				}

		}
}