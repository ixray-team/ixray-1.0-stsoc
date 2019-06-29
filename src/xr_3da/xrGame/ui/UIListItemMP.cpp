// File:        UIListItemMP.cpp
// Description: List item for multiplayer dialog (servers list)
// Created:     22.12.2004
// Author:      Serhiy 0. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "StdAfx.h"
#include "UIListItemMP.h"
#include "../HUDManager.h"
#include "../level.h"

CUIListItemMP::CUIListItemMP(){
	// set textures
	InitTexture("ui\\hud_map_point");
	SetStretchTexture(true);
	SetColor(color_argb(0, 0, 0, 0));

	// attach all children
	AttachChild(&m_sServer);
	AttachChild(&m_sMap);
	AttachChild(&m_sMode);
	AttachChild(&m_sPlayers);
	AttachChild(&m_sPing);

	// Server
	m_sServer.SetFont(HUD().Font().pFontMedium);
	m_sServer.SetTextColor(color_argb(255, 255, 255, 255));
	m_sServer.SetTextAlignment(CGameFont::alLeft);
	m_sServer.Init(0,0,0,0);
	m_sServer.SetElipsis(CUIStatic::eepNone, 1);

	// Server
	m_sMap.SetFont(HUD().Font().pFontMedium);
	m_sMap.SetTextColor(color_argb(255, 255, 255, 255));
	m_sMap.SetTextAlignment(CGameFont::alLeft);
	m_sMap.Init(0,0,0,0);
	m_sMap.SetElipsis(CUIStatic::eepNone, 1);

	// Server
	m_sMode.SetFont(HUD().Font().pFontMedium);
	m_sMode.SetTextColor(color_argb(255, 255, 255, 255));
	m_sMode.SetTextAlignment(CGameFont::alLeft);
	m_sMode.Init(0,0,0,0);
	m_sMode.SetElipsis(CUIStatic::eepNone, 1);

	
}

CUIListItemMP::~CUIListItemMP(){

}

void CUIListItemMP::Init(float x, float y, float width, float height){
	CUIListItem::Init(x, y, width, height);	
}

void CUIListItemMP::SetText(LPCSTR sServer, LPCSTR sMap, LPCSTR sMode, LPCSTR sPlayers, LPCSTR sPing){
	m_sServer.SetText(sServer);
	m_sMap.SetText(sMap);
	m_sMode.SetText(sMode);
	m_sPlayers.SetText(sPlayers);
	m_sPing.SetText(sPing);
}
