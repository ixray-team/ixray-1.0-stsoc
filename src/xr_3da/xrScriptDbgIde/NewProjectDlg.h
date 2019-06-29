#pragma once


// CNewProjectDlg dialog

class CNewProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewProjectDlg)

public:
	CNewProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProjectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_PROJECT };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	CString m_folder;
	CString m_project_name;
public:
	CString GetProjectName(void);
	void SetFileName(CString& str);
	afx_msg void OnBnClickedButton1();
};
