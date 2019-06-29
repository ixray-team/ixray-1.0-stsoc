#include "stdafx.h"
#include "Text_Console.h"

//ENGINE_API CTextConsole* TextConsole = NULL;
extern	const char *			ioc_prompt;

CTextConsole::CTextConsole()
{
	m_pMainWnd = NULL;
	m_hConsoleWnd = NULL;
	m_hLogWnd = NULL;
	m_hLogWndFont = NULL;

	m_bScrollLog = true;
	m_dwStartLine = 0;
}

CTextConsole::~CTextConsole()
{
	m_pMainWnd = NULL;
};
//-------------------------------------------------------------------------------------------
LRESULT CALLBACK TextConsole_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void	CTextConsole::CreateConsoleWnd()
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
	//----------------------------------
	RECT cRc;
	GetClientRect(*m_pMainWnd, &cRc);
	INT lX = cRc.left;
	INT lY = cRc.top;
	INT lWidth = cRc.right - cRc.left;
	INT lHeight = cRc.bottom - cRc.top;
	//----------------------------------
	const char*	wndclass ="TEXT_CONSOLE";

	// Register the windows class
	WNDCLASS wndClass = { 0, TextConsole_WndProc, 0, 0, hInstance,
		NULL,
		LoadCursor( hInstance, IDC_ARROW ),
		GetStockBrush(GRAY_BRUSH),
		NULL, wndclass };
	RegisterClass( &wndClass );

	// Set the window's initial style
	u32 dwWindowStyle = WS_OVERLAPPED | WS_CHILD | WS_VISIBLE;// | WS_CLIPSIBLINGS;// | WS_CLIPCHILDREN;

	// Set the window's initial width
	RECT rc;
	SetRect			( &rc, lX, lY, lWidth, lHeight );
//	AdjustWindowRect( &rc, dwWindowStyle, FALSE );

	// Create the render window
	m_hConsoleWnd = CreateWindow( wndclass, "XRAY Text Console", dwWindowStyle,
		lX, lY,
		lWidth, lHeight, *m_pMainWnd,
		0, hInstance, 0L );
	//---------------------------------------------------------------------------
	R_ASSERT2(m_hConsoleWnd, "Unable to Create TextConsole Window!");
};
//-------------------------------------------------------------------------------------------
LRESULT CALLBACK TextConsole_LogWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void	CTextConsole::CreateLogWnd()
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
	//----------------------------------
	RECT cRc;
	GetClientRect(m_hConsoleWnd, &cRc);
	INT lX = cRc.left;
	INT lY = cRc.top;
	INT lWidth = cRc.right - cRc.left;
	INT lHeight = cRc.bottom - cRc.top;
	//----------------------------------
	const char*	wndclass ="TEXT_CONSOLE_LOG_WND";

	// Register the windows class
	WNDCLASS wndClass = { 0, TextConsole_LogWndProc, 0, 0, hInstance,
		NULL,
		LoadCursor( NULL, IDC_ARROW ),
		GetStockBrush(BLACK_BRUSH),
		NULL, wndclass };
	RegisterClass( &wndClass );

	// Set the window's initial style
	u32 dwWindowStyle = WS_OVERLAPPED | WS_CHILD | WS_VISIBLE;// | WS_CLIPSIBLINGS;
//	u32 dwWindowStyleEx = WS_EX_CLIENTEDGE;

	// Set the window's initial width
	RECT rc;
	SetRect			( &rc, lX, lY, lWidth, lHeight );
//	AdjustWindowRect( &rc, dwWindowStyle, FALSE );

	// Create the render window
	m_hLogWnd = CreateWindow(wndclass, "XRAY Text Console Log", dwWindowStyle,
		lX, lY,
		lWidth, lHeight, m_hConsoleWnd,
		0, hInstance, 0L );
	//---------------------------------------------------------------------------
	R_ASSERT2(m_hLogWnd, "Unable to Create TextConsole Window!");
	//---------------------------------------------------------------------------
	ShowWindow(m_hLogWnd, SW_SHOW); 
	UpdateWindow(m_hLogWnd);
	//-----------------------------------------------
	LOGFONT lf; 
	lf.lfHeight = -12; 
	lf.lfWidth = 0;
	lf.lfEscapement = 0; 
	lf.lfOrientation = 0; 
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0; 
	lf.lfUnderline = 0; 
	lf.lfStrikeOut = 0; 
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_STRING_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;	
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	sprintf(lf.lfFaceName,"");

	m_hLogWndFont = CreateFontIndirect(&lf);
	R_ASSERT2(m_hLogWndFont, "Unable to Create Font for Log Window");
	//------------------------------------------------
	m_hDC_LogWnd = GetDC(m_hLogWnd);
	R_ASSERT2(m_hDC_LogWnd, "Unable to Get DC for Log Window!");
	//------------------------------------------------
	m_hDC_LogWnd_BackBuffer = CreateCompatibleDC(m_hDC_LogWnd);
	R_ASSERT2(m_hDC_LogWnd_BackBuffer, "Unable to Create Compatible DC for Log Window!");
	//------------------------------------------------
	GetClientRect(m_hLogWnd, &cRc);
	lWidth = cRc.right - cRc.left;
	lHeight = cRc.bottom - cRc.top;
	//----------------------------------
	m_hBB_BM = CreateCompatibleBitmap(m_hDC_LogWnd, lWidth, lHeight);
	R_ASSERT2(m_hBB_BM, "Unable to Create Compatible Bitmap for Log Window!");
	//------------------------------------------------
	m_hOld_BM = (HBITMAP)SelectObject(m_hDC_LogWnd_BackBuffer, m_hBB_BM);
	//------------------------------------------------
	m_hPrevFont = (HFONT)SelectObject(m_hDC_LogWnd_BackBuffer, m_hLogWndFont);
	//------------------------------------------------
	SetTextColor(m_hDC_LogWnd_BackBuffer, RGB(255, 255, 255));
	SetBkColor(m_hDC_LogWnd_BackBuffer, RGB(0, 0, 0));
	//------------------------------------------------
	m_hBackGroundBrush = GetStockBrush(BLACK_BRUSH);
}

void	CTextConsole::Initialize	()
{
	CConsole::Initialize();
	//----------------------------------
	m_pMainWnd = &Device.m_hWnd;
	//----------------------------------
	CreateConsoleWnd();
	//-----------------------------------
	CreateLogWnd();
	//-----------------------------------------------
	ShowWindow(m_hConsoleWnd, SW_SHOW); 
	UpdateWindow(m_hConsoleWnd);	
	//-----------------------------------
};

void	CTextConsole::Destroy		()
{
	CConsole::Destroy();	
	//-------------------------------------
	SelectObject(m_hDC_LogWnd_BackBuffer, m_hPrevFont);
	SelectObject(m_hDC_LogWnd_BackBuffer, m_hOld_BM);

	if (m_hBB_BM) DeleteObject(m_hBB_BM);
	if (m_hOld_BM) DeleteObject(m_hOld_BM);
	if (m_hLogWndFont) DeleteObject (m_hLogWndFont);
	if (m_hPrevFont) DeleteObject(m_hPrevFont);
	if (m_hBackGroundBrush) DeleteObject(m_hBackGroundBrush);

	ReleaseDC(m_hLogWnd, m_hDC_LogWnd_BackBuffer);
	ReleaseDC(m_hLogWnd, m_hDC_LogWnd);
	//-------------------------------------
	DestroyWindow(m_hLogWnd);
	//-------------------------------------
	DestroyWindow(m_hConsoleWnd);	
}

void	CTextConsole::OnPaint()
{
	RECT wRC;
	GetClientRect(m_hLogWnd, &wRC);
	//------------------------------
	PAINTSTRUCT ps;
	BeginPaint(m_hLogWnd, &ps);
	//------------------------------
	DrawLog(m_hDC_LogWnd_BackBuffer);
	BitBlt(	m_hDC_LogWnd, 
			wRC.left, wRC.top,
			wRC.right - wRC.left, wRC.bottom - wRC.top,
			m_hDC_LogWnd_BackBuffer,
			0, 0,
			SRCCOPY);
	//------------------------------
	EndPaint(m_hLogWnd, &ps);
}

void	CTextConsole::DrawLog(HDC hDC)
{
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	//-------------------------------------------------
	RECT wRC;
	GetClientRect(m_hLogWnd, &wRC);
	//-------------------------------------------------
	FillRect(hDC, &wRC, m_hBackGroundBrush);
	//-------------------------------------------------
//	INT Width = wRC.right - wRC.left;
	INT Height = wRC.bottom - wRC.top;
	//---------------------------------------------------------------------------------
	char		buf	[MAX_LEN+5];
	strcpy		(buf,ioc_prompt);
	strcat		(buf,editor);
	if (bCursor) strcat(buf,"|");
	SetTextColor(hDC, RGB(128, 128, 255));
	TextOut(hDC, 0, Height-tm.tmHeight, buf, xr_strlen(buf));

	INT YPos = Height - tm.tmHeight - tm.tmHeight;
	for (int i=LogFile.size()-1-scroll_delta; i>=0; i--) 
	{
		YPos-=tm.tmHeight;
		if (YPos<0)	break;
		LPCSTR Str = *LogFile[i];
		LPCSTR pOut = Str;
		if (!Str) continue;
		switch (Str[0])
		{
		case '~':
			SetTextColor(hDC, RGB(0, 0, 255));
			pOut = Str + 2;
			break;
		case '!':
			SetTextColor(hDC, RGB(255, 0, 0));
			pOut = Str + 2;
			break;
		case '*':
			SetTextColor(hDC, RGB(128, 128, 128));
			pOut = Str + 2;
			break;
		case '-':
			SetTextColor(hDC, RGB(0, 255, 0));
			pOut = Str + 2;
			break;
		default:
			SetTextColor(hDC, RGB(255, 255, 255));
			break;
		}
		BOOL res = TextOut(hDC, 0, YPos, pOut,xr_strlen(pOut));
		if (!res)
		{
			R_ASSERT(0);
		}
	};	
};

void	CTextConsole::OnRender			(void)
{
	//OnPaint();
};

void	CTextConsole::OnFrame			(void)
{
	inherited::OnFrame();
	InvalidateRect(m_hConsoleWnd, NULL, FALSE);
	SetCursor(LoadCursor( NULL, IDC_ARROW ));
}
