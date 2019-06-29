// File:        UIListWndEx.h
// Description: This List Box compatible with ListItemEx.
//              Requiered to use feature "Selected Item"
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#pragma once
#include ".\uilistwnd.h"
#include "UIListItemEx.h"

class CUIListWndEx: public CUIListWnd {
public:
	CUIListWndEx();
	virtual ~CUIListWndEx();
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	CUIListItemEx*	GetExItem(int index);
//	virtual bool	AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont, bool doPreProcess);
	
	DECLARE_SCRIPT_REGISTER_FUNCTION

protected:

};

add_to_type_list(CUIListWndEx)
#undef script_type_list
#define script_type_list save_type_list(CUIListWndEx)