// GotoLineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GotoLineDialog.h"

// CGotoLineDialog dialog

IMPLEMENT_DYNAMIC(CGotoLineDialog, CDialog)
CGotoLineDialog::CGotoLineDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGotoLineDialog::IDD, pParent)
	, m_line_n(0)
{
}

CGotoLineDialog::~CGotoLineDialog()
{
}

void CGotoLineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_line_n);
	DDV_MinMaxInt(pDX, m_line_n, 1, 100000);
}


BEGIN_MESSAGE_MAP(CGotoLineDialog, CDialog)
END_MESSAGE_MAP()


// CGotoLineDialog message handlers
