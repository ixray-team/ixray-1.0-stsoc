// ScintillaBar.h: interface for the CScintillaBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCINTILLABAR_H__377FAA54_BEAC_412F_8A77_86A79CBF577C__INCLUDED_)
#define AFX_SCINTILLABAR_H__377FAA54_BEAC_412F_8A77_86A79CBF577C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScintillaView;
struct SCNotification;

class CScintillaBar : public CCJTabCtrlBar
{
public:
	CScintillaBar();
	virtual ~CScintillaBar();

	virtual int OnSci(CScintillaView* pView, SCNotification* pNotify) = 0;
};

#endif // !defined(AFX_SCINTILLABAR_H__377FAA54_BEAC_412F_8A77_86A79CBF577C__INCLUDED_)
