#pragma once
#include "resource.h"

// CDebugDialog dialog

class CDebugDialog : public CDialog
{
	DECLARE_DYNAMIC(CDebugDialog)

public:
	CDebugDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugDialog();

// Dialog Data
	enum { IDD = IDD_DEBUG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// command
	CString m_command;
	// arguments
	CString m_arguments;
	// working directory
	CString m_working_dir;
};
