// DebugDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DebugDialog.h"


// CDebugDialog dialog

IMPLEMENT_DYNAMIC(CDebugDialog, CDialog)
CDebugDialog::CDebugDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugDialog::IDD, pParent)
	, m_command(_T(""))
	, m_arguments(_T(""))
	, m_working_dir(_T(""))
{
}

CDebugDialog::~CDebugDialog()
{
}

void CDebugDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CMD, m_command);
	DDX_Text(pDX, IDC_EDIT_ARGS, m_arguments);
	DDX_Text(pDX, IDC_EDIT_WDIR, m_working_dir);
}


BEGIN_MESSAGE_MAP(CDebugDialog, CDialog)
END_MESSAGE_MAP()


// CDebugDialog message handlers
