#include "stdafx.h"
#pragma hdrstop

#include "xr_input.h"
#include "fcontroller.h"

void	CController::iCapture					(void)
{
	VERIFY(pInput);
	pInput->iCapture(this);
}

void	CController::iRelease					(void)
{
	VERIFY(pInput);
	pInput->iRelease(this);
}

void	CController::iGetLastMouseDelta			(Ivector2& p)
{
	VERIFY(pInput);
	pInput->iGetLastMouseDelta( p );
}

void CController::OnInputDeactivate				(void)
{
	int i;
	for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++ )
		if (iGetKeyState(i))	OnKeyboardRelease	(i);
	for (i = 0; i < CInput::COUNT_MOUSE_BUTTONS; i++ )
		OnMouseRelease( i );
	OnMouseStop	( DIMOFS_X, 0 );
	OnMouseStop	( DIMOFS_Y, 0 );
}

BOOL CController::iGetKeyState(int dik)
{
	VERIFY(pInput);
	return pInput->iGetAsyncKeyState(dik);
}

BOOL CController::iGetBtnState(int btn)
{
	VERIFY(pInput);
	return pInput->iGetAsyncBtnState(btn);
}

void	CController::iGetMousePosScreen			(Ivector2& p)
{
	GetCursorPos((LPPOINT)&p);
}
void	CController::iGetMousePosReal			(HWND hwnd, Ivector2 &p)
{
	iGetMousePosScreen(p);
	if (hwnd) ScreenToClient(hwnd,(LPPOINT)&p);
}
void	CController::iGetMousePosReal			(Ivector2 &p)
{
	iGetMousePosReal(Device.m_hWnd,p);
}
void	CController::iGetMousePosIndependent		(Fvector2 &f)
{
	Ivector2 p;
	iGetMousePosReal(p);
	f.set(
		2.f*float(p.x)/float(Device.dwWidth)-1.f,
		2.f*float(p.y)/float(Device.dwHeight)-1.f
		);
}
void	CController::iGetMousePosIndependentCrop	(Fvector2 &f)
{
	iGetMousePosIndependent(f);
	if (f.x<-1.f) f.x=-1.f;
	if (f.x> 1.f) f.x= 1.f;
	if (f.y<-1.f) f.y=-1.f;
	if (f.y> 1.f) f.y= 1.f;
}
