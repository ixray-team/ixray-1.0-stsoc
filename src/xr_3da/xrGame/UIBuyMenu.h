#ifndef __XR_UIBUYMENU_H__
#define __XR_UIBUYMENU_H__
#pragma once

#include "UICustomMenu.h"
// refs
class CUIGameCustom;

class CUIBuyMenu: public CUICustomMenu{
	float				menu_offs_row;
	float				menu_offs_col[2];
public:
						CUIBuyMenu			();

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int /**dik/**/){return false;}
	virtual bool		IR_OnMouseMove			(int /**dx/**/, int /**dy/**/){return false;}
};

#endif // __XR_UIBUYMENU_H__
