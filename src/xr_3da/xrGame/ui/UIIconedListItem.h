//=============================================================================
//  Filename:   UIIconedListItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Лист айтем с иконкой в начале первой строки текста
//=============================================================================

#ifndef UI_ICONED_LIST_ITEM_H_
#define UI_ICONED_LIST_ITEM_H_

#pragma once

#include "UIListItem.h"
#include "../UIStaticItem.h"

//////////////////////////////////////////////////////////////////////////

class CUIIconedListItem: public CUIListItem
{
	typedef CUIListItem inherited;
public:
	CUIIconedListItem(): m_uIconSize		(0),
						 m_bTexturePresent	(false),
						 m_bIconDraw		(true)
	{}

	// Устанавливаем иконку и ее размер
	void			SetIcon(shared_str textureName, u32 uIconSize);
	virtual void	Draw();
	virtual void	Show(bool status);
	// После смены позиции листа обновить положение иконочки
	virtual void	Update();

	// Смещение иконки
	void SetIconOffset(const float oX, const float oY);
protected:
	CUIStaticItem	m_Icon;
	// Размер иконки
	u32				m_uIconSize;
	// Иконка присутствует
	bool			m_bTexturePresent;
	// Иконка отображается
	bool			m_bIconDraw;
	// Запоминаем предыдущий AbsoluteRect
	Frect			m_OldAbsoluteWndRect;
};

#endif	//UI_ICONED_LIST_ITEM_H_