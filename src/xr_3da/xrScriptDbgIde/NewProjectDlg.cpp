// NewProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NewProjectDlg.h"
#include ".\newprojectdlg.h"


// CNewProjectDlg dialog

IMPLEMENT_DYNAMIC(CNewProjectDlg, CDialog)
CNewProjectDlg::CNewProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewProjectDlg::IDD, pParent)
	, m_project_name(_T(""))
	, m_folder(_T(""))
{
}

CNewProjectDlg::~CNewProjectDlg()
{
}

void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_project_name);
	DDV_MaxChars(pDX, m_project_name, 64);
	DDX_Text(pDX, IDC_EDIT2, m_folder);
}


BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CNewProjectDlg message handlers

CString CNewProjectDlg::GetProjectName(void)
{
	CString res;
	int sz = m_folder.GetLength();
	if (sz!=0){
		int p = m_folder.Find("\\", sz-1 );
		if(p != sz-1 )
			m_folder = m_folder + "\\";
	}
	CString src = m_folder + m_project_name;
	if(src.Find(".lpr")!=src.GetLength()-4)
		res = src + ".lpr";

	return res;
}

void CNewProjectDlg::SetFileName(CString& str)
{
	m_project_name = str;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg) 
	{
		case BFFM_INITIALIZED: 
			if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir)) 
			{
				// WParam is TRUE since you are passing a path.
				// It would be FALSE if you were passing a pidl.
				SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
			}
			break;
		case BFFM_SELCHANGED: 
			// Set the status window to the currently selected path.
			if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
			break;
		default:
			break;
	}
	
	return 0;
}


void CNewProjectDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;

		pidl = SHBrowseForFolder(&bi);
		if (pidl) 
		{
			if (SHGetPathFromIDList(pidl,szDir)) 
			{
				m_folder = szDir;
				UpdateData(FALSE);
			}

			// In C++: pMalloc->Free(pidl); pMalloc->Release();
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}	
}
