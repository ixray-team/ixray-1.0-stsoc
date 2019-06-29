#pragma once
#include "resource.h"

// CSSOptionsDlg dialog

class CSSOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSSOptionsDlg)

public:
	CSSOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSSOptionsDlg();

// Dialog Data
	enum { IDD = IDD_SS_OPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_ss_ini_name;
	CString m_ss_folder;
	CString m_ss_username;
	CString m_ss_userpass;
	afx_msg void OnBnClickedButtonSelSsIni();
	afx_msg void OnBnClickedCheckNoSS();
	BOOL m_no_ss;
};
