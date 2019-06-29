// OutputWnd.h: interface for the COutputWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTWND_H__804E57BF_EABF_48F9_B600_90503B44A217__INCLUDED_)
#define AFX_OUTPUTWND_H__804E57BF_EABF_48F9_B600_90503B44A217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScintillaView;
struct SCNotification;

#include "ScintillaBar.h"

class COutputWnd : public CScintillaBar
{
public:
	void GotoLine(CString strLine);
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT, LPCSTR caption="");
	CScintillaView* GetOutput(int nOutput) { return (CScintillaView*)GetView(nOutput); };
	void SetActiveOutput(int nOutput) { if ( GetActiveView()!=GetView(nOutput) ) SetActiveView(nOutput); };

	COutputWnd();
	virtual ~COutputWnd();

	enum
	{
//		outputBuild = 0,
		outputDebug = 0
//		outputFiF
	} outputTypes;

	virtual int OnSci(CScintillaView* pView, SCNotification* pNotify);

protected:
	int OnBuildSci(SCNotification* pNotify);
	int OnDebugSci(SCNotification* pNotify);
};

#endif // !defined(AFX_OUTPUTWND_H__804E57BF_EABF_48F9_B600_90503B44A217__INCLUDED_)
