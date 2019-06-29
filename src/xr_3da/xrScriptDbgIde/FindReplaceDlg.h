#pragma once

class CScintillaFindReplaceDlg : public CFindReplaceDialog
{
public:
//Constructors / Destructors
  CScintillaFindReplaceDlg();

//Methods
  BOOL Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith = NULL, DWORD dwFlags = FR_DOWN, CWnd* pParentWnd = NULL);
  BOOL GetRegularExpression() const { return m_bRegularExpression; };
  void SetRegularExpression(BOOL bRegularExpression) { m_bRegularExpression = bRegularExpression; };

protected:
	//{{AFX_MSG(CScintillaFindReplaceDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
  afx_msg void OnRegularExpression();
  
//Member variables
  BOOL m_bRegularExpression;

	DECLARE_MESSAGE_MAP()
};

