// WorkspaceWnd.h: interface for the CWorkspaceWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKSPACEWND_H__1609FA7C_19A2_49B5_A86E_65E866932D97__INCLUDED_)
#define AFX_WORKSPACEWND_H__1609FA7C_19A2_49B5_A86E_65E866932D97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeViewFiles;

class CWorkspaceWnd : public CCJTabCtrlBar
{
public:
	void Enable(BOOL bEnable);
	CTreeViewFiles* GetTreeViewFiles();
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT);
	CWorkspaceWnd();
	virtual ~CWorkspaceWnd();

protected:
	CImageList	m_tabImages;

};

#endif // !defined(AFX_WORKSPACEWND_H__1609FA7C_19A2_49B5_A86E_65E866932D97__INCLUDED_)
