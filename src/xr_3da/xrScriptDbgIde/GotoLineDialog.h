#pragma once
#include "resource.h"


// CGotoLineDialog dialog

class CGotoLineDialog : public CDialog
{
	DECLARE_DYNAMIC(CGotoLineDialog)

public:
	CGotoLineDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGotoLineDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_line_n;
};
