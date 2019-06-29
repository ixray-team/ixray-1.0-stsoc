#pragma once
#include "XR_IOConsole.h"

class ENGINE_API CTextConsole :
	public CConsole
{
	typedef CConsole inherited ;
private:
	HWND*	m_pMainWnd;

	HWND	m_hConsoleWnd;
	void	CreateConsoleWnd	();
	
	HWND	m_hLogWnd;	
	void	CreateLogWnd		();

	bool	m_bScrollLog;
	u32		m_dwStartLine;
	void	DrawLog(HDC hDC);
private:
	HFONT	m_hLogWndFont;
	HFONT	m_hPrevFont;
	HBRUSH	m_hBackGroundBrush;

	HDC		m_hDC_LogWnd;
	HDC		m_hDC_LogWnd_BackBuffer;
	HBITMAP m_hBB_BM, m_hOld_BM;
public:
	CTextConsole();
	virtual ~CTextConsole();

	virtual	void	Initialize	();
	virtual	void	Destroy		();

	void	AddString	(LPCSTR string);
	void	OnPaint		();

	virtual void	OnRender			(void);
	virtual void	OnFrame				(void);
};

//extern ENGINE_API CTextConsole* TextConsole;