// ProjectProperties.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "ProjectProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectProperties dialog


CProjectProperties::CProjectProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectProperties)
	m_bGenerateListing = FALSE;
	m_strOutputDir = _T("");
	m_strOutputPrefix = _T("");
	m_strIntermediateDir = _T("");
	//}}AFX_DATA_INIT
}


void CProjectProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectProperties)
	DDX_Check(pDX, IDC_GENERATE_LISTING, m_bGenerateListing);
	DDX_Text(pDX, IDC_OUTPUT_DIR, m_strOutputDir);
	DDX_Text(pDX, IDC_OUTPUT_PREFIX, m_strOutputPrefix);
	DDX_Text(pDX, IDC_INTERMEDIATE_DIR, m_strIntermediateDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectProperties, CDialog)
	//{{AFX_MSG_MAP(CProjectProperties)
	ON_BN_CLICKED(IDC_BROWSE_FOLDER, OnBrowseFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectProperties message handlers
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

void CProjectProperties::OnBrowseFolder() 
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
				m_strOutputDir = szDir;
				UpdateData(FALSE);
			}

			// In C++: pMalloc->Free(pidl); pMalloc->Release();
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}	
}
