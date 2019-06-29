// File:		UIPdaKillMessage.cpp
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#include "StdAfx.h"
#include "UIPdaKillMessage.h"
#include "UIInventoryUtilities.h"

using namespace InventoryUtilities;

const int INDENT = 3;

CUIPdaKillMessage::CUIPdaKillMessage(){
	AttachChild(&m_victim_name);
	AttachChild(&m_killer_name);	
	AttachChild(&m_initiator);
	AttachChild(&m_ext_info);
}

CUIPdaKillMessage::~CUIPdaKillMessage(){

}

void CUIPdaKillMessage::Init(KillMessageStruct& msg){
#ifdef DEBUG
	R_ASSERT2(GetWidth(),  "CUIPdaKillMessage::Init(msg) - need to call ::Init(x, y, width, height) before");
	R_ASSERT2(GetHeight(), "CUIPdaKillMessage::Init(msg) - need to call ::Init(x, y, width, height) before");
#endif	

	float x		= 0;
	float width	= 0;

	width = InitText(m_killer_name, x, msg.m_killer);		x += width ? width + INDENT : 0;
	width = InitIcon(m_initiator,   x, msg.m_initiator);	x += width ? width + INDENT : 0;
	width = InitText(m_victim_name, x, msg.m_victim);		x += width ? width + INDENT : 0;
			InitIcon(m_ext_info,	x, msg.m_ext_info);
}

float CUIPdaKillMessage::InitText(CUIStatic& refStatic, float x, PlayerInfo& info){

	if ( 0 == xr_strlen(info.m_name))
		return 0;

	float		y = 0;
	float		selfHeight = GetHeight();
	CGameFont*	pFont = GetFont();

	float width = pFont->SizeOf_/*Rel*/(*info.m_name);
	float height = pFont->CurrentHeight_();
	y = (selfHeight - height)/2;

	clamp(width, 0.0f, 110.0f);
	refStatic.Init(x, y, width + 1, height);
	refStatic.SetElipsis(CUIStatic::eepEnd, 0);
	refStatic.SetText(*info.m_name);
	refStatic.SetTextColor(info.m_color);

	return width;
}

void CUIPdaKillMessage::SetTextColor(u32 color){	
	m_victim_name.SetTextColor(subst_alpha(m_victim_name.GetTextColor(),color_get_A(color)));
	m_killer_name.SetTextColor(subst_alpha(m_killer_name.GetTextColor(),color_get_A(color)));
	CUIStatic::SetTextColor(color);
}

void CUIPdaKillMessage::SetColor(u32 color){	
	m_initiator.SetColor(color);
	m_ext_info.SetColor(color);
	CUIStatic::SetColor(color);
}

float CUIPdaKillMessage::InitIcon(CUIStatic& refStatic, float x, IconInfo& info){
	if ( 0 == info.m_rect.width())
		return 0;

	if (info.m_shader == NULL)
		return 0;

	float		y = 0;
	float		selfHeight = GetHeight();
	float		scale = 0;
	Frect		rect = info.m_rect;

	float width = rect.width();
	float height = rect.height();
	
	scale = selfHeight/height;
	if (scale > 1)
		scale = 1;
	width  = width*scale;
	height = height*scale;
	y = (selfHeight - height) /2;
	refStatic.Init(x, y, width, height);
	refStatic.SetOriginalRect(info.m_rect);
	refStatic.SetShader(info.m_shader);
	refStatic.SetStretchTexture(true);

	return width;
}

void CUIPdaKillMessage::SetFont(CGameFont* pFont){
	m_victim_name.SetFont(pFont);
	m_killer_name.SetFont(pFont);
	CUIStatic::SetFont(pFont);
}