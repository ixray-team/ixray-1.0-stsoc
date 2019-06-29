#include "stdafx.h"
#include "Text_Console.h"

LRESULT CALLBACK TextConsole_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_PAINT:
		{

			CTextConsole* pTextConsole = (CTextConsole*)Console;
			pTextConsole->OnPaint();
//			Console->OnPaint();
		}break;
	default:
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK TextConsole_LogWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_ERASEBKGND:
		return (LRESULT)1; // Say we handled it.

	case WM_PAINT:
		{
		}break;
	default:
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}