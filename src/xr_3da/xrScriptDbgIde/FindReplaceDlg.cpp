#include "stdafx.h"
#include "FindReplaceDlg.h"

#include "resource.h"

////////////////////////////////// Implementation /////////////////////////////

BEGIN_MESSAGE_MAP(CScintillaFindReplaceDlg, CFindReplaceDialog)
	//{{AFX_MSG_MAP(CScintillaFindReplaceDlg)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_REGULAR_EXPRESSION, OnRegularExpression)
END_MESSAGE_MAP()

CScintillaFindReplaceDlg::CScintillaFindReplaceDlg()
{
  m_bRegularExpression = FALSE;
}

BOOL CScintillaFindReplaceDlg::Create(BOOL bFindDialogOnly,	LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith, DWORD dwFlags, CWnd* pParentWnd)
{
  //Tell Windows to use our dialog instead of the standard one
  m_fr.hInstance = AfxGetResourceHandle();
  m_fr.Flags |= FR_ENABLETEMPLATE;
  if (bFindDialogOnly)
    m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_SCINTILLA_FINDDLGORD);
  else
    m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_SCINTILLA_REPLACEDLGORD);

  //Let the base class do its thing
  return CFindReplaceDialog::Create(bFindDialogOnly,	lpszFindWhat, lpszReplaceWith, dwFlags, pParentWnd);
}

BOOL CScintillaFindReplaceDlg::OnInitDialog() 
{
  //let the base class do its thing
	BOOL bReturn = CFindReplaceDialog::OnInitDialog();

  //Should we check the regular expression check box
  CButton* pCtrl = (CButton*) GetDlgItem(IDC_REGULAR_EXPRESSION);
  ASSERT(pCtrl);
  pCtrl->SetCheck(m_bRegularExpression);
	
	return bReturn;
}

void CScintillaFindReplaceDlg::OnRegularExpression() 
{
  //Save the state of the Regular expression checkbox into a member variable
  CButton* pCtrl = (CButton*) GetDlgItem(IDC_REGULAR_EXPRESSION);
  ASSERT(pCtrl);
	m_bRegularExpression = (pCtrl->GetCheck() == 1);
}

