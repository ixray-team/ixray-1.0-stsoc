// SSOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSOptionsDlg.h"
#include ".\ssoptionsdlg.h"


// CSSOptionsDlg dialog

IMPLEMENT_DYNAMIC(CSSOptionsDlg, CDialog)
CSSOptionsDlg::CSSOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSSOptionsDlg::IDD, pParent)
	, m_ss_ini_name(_T(""))
	, m_ss_folder(_T(""))
	, m_ss_username(_T(""))
	, m_ss_userpass(_T(""))
	, m_no_ss(FALSE)
{
}

CSSOptionsDlg::~CSSOptionsDlg()
{
}

void CSSOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SS_INI_FILE, m_ss_ini_name);
	DDX_Text(pDX, IDC_EDIT_SS_FOLDER, m_ss_folder);
	DDX_Text(pDX, IDC_EDIT_SS_USER_NAME, m_ss_username);
	DDX_Text(pDX, IDC_EDIT_SS_USER_PASS, m_ss_userpass);
	DDX_Check(pDX, IDC_CHECK1, m_no_ss);
}


BEGIN_MESSAGE_MAP(CSSOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEL_SS_INI, OnBnClickedButtonSelSsIni)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheckNoSS)
END_MESSAGE_MAP()


void CSSOptionsDlg::OnBnClickedButtonSelSsIni()
{
//	CFileDialog dlg(TRUE,"ini","srcsafe.ini",0,"*.ini");
	CFileDialog dlg(TRUE, NULL, "srcsafe.ini", OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, 
		"SourceSafe ini files (*.ini)|*.ini|All files (*.*)|*.*||");
	if(dlg.DoModal()==IDOK)
	{
		m_ss_ini_name = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSSOptionsDlg::OnBnClickedCheckNoSS()
{
	UpdateData(TRUE);
	if(m_no_ss){
		m_ss_ini_name="";
		m_ss_folder="";
		m_ss_username="";
		m_ss_userpass="";
		UpdateData(FALSE);
	}

}
