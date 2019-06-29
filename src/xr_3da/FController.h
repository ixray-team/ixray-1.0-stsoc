// CController.h: interface for the CController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

class ENGINE_API	CController
{
public:
	void			iGetLastMouseDelta			(Ivector2& p);
	void			iGetMousePosScreen			(Ivector2& p);
	void			iGetMousePosReal			(HWND hwnd, Ivector2 &p);
	void			iGetMousePosReal			(Ivector2 &p);
	void			iGetMousePosIndependent		(Fvector2 &f);
	void			iGetMousePosIndependentCrop	(Fvector2 &f);
	BOOL			iGetKeyState				(int dik);
	BOOL			iGetBtnState				(int btn);
	void			iCapture					(void);
	void			iRelease					(void);

	virtual void	OnInputDeactivate			(void);
	virtual void	OnInputActivate				(void)			{};

	virtual void	OnMousePress				(int btn)		{};
	virtual void	OnMouseRelease				(int btn)		{};
	virtual void	OnMouseHold					(int btn)		{};
	virtual void	OnMouseMove					(int x, int y)	{};
	virtual void	OnMouseStop					(int x, int y)	{};

	virtual void	OnKeyboardPress				(int dik)		{};
	virtual void	OnKeyboardRelease			(int dik)		{};
	virtual void	OnKeyboardHold				(int dik)		{};
};

#endif // !defined(AFX_FCONTROLLER_H__A898DA00_FB63_11D3_B4E3_4854E82A090D__INCLUDED_)
