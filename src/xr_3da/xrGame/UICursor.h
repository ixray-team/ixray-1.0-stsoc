#ifndef __XR_UICURSOR_H__
#define __XR_UICURSOR_H__
#pragma once

#include "UIStaticItem.h"

class CUICursor:public pureRender
{
	ref_shader		hShader;
	bool			bVisible;
	bool			bHoldMode;
	Fvector2		vPos;
	Fvector2		vPrevPos;
	Fvector2		vHoldPos;

	CUIStaticItem	m_si;	
	float			m_fSensitivity;
public:
					CUICursor		();
	virtual			~CUICursor		();
	virtual void	OnRender		();
	void			HoldMode		(bool b);
	Fvector2		GetPosDelta		();
	void			SetPos			(float x, float y);
	void			GetPos			(float& x, float& y);
	Fvector2		GetPos			();
	void			MoveBy			(float dx, float dy);

	bool			IsVisible		() {return bVisible;}
	void			Show			() {bVisible = true;}
	void			Hide			() {bVisible = false;}


	void SetSensitivity(float sens) {m_fSensitivity = sens;}
	float GetSensitivity() {return m_fSensitivity;}

public:
	typedef fastdelegate::FastDelegate1<Fvector2>	CURSOR_MOVE_EVENT;
	CURSOR_MOVE_EVENT				m_cursor_move_event;
	void			SetMoveReceiver(CURSOR_MOVE_EVENT cb){m_cursor_move_event = cb;};

};

#endif //__XR_UICURSOR_H__
