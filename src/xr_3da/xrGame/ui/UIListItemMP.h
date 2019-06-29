// File:        UIListItemMP.h
// Description: ListItem for multiplayer dialog (for servers list)
// Created:     22.12.2004
// Author:      Serhiy 0. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#pragma once
#include "UIListItem.h"

#define SERVER_WIDTH smart_cast<int>((1.0/5)*GetWidth())

class CUIListItemMP : CUIListItem {
public:
	CUIListItemMP();
	virtual ~CUIListItemMP();

	virtual void Init(float x, float y, float width, float height);
	virtual void SetText(LPCSTR sServer, LPCSTR sMap, LPCSTR sMode, LPCSTR sPlayers, LPCSTR sPing);
	//virtual void SetHighlightText(bool bHighlight);

private:
	CUIListItem m_sServer;
	CUIListItem m_sMap;
	CUIListItem m_sMode;
	CUIListItem m_sPlayers;
	CUIListItem m_sPing;
};