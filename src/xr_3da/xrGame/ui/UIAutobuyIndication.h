// File:		UIAutobuyIndication.h
// Description:	Indication for 3 presets of autobuy command. for UIBuyWeaponWnd
// Created:		21.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua


// Copyright 2005 GSC Game World

#pragma once

#include "UIStatic.h"

class CUIAutobuyIndication : public CUIWindow {
public:
	CUIAutobuyIndication();
	~CUIAutobuyIndication();

	virtual void Init(float x, float y, float width, float height);

	void SetPrices(int p1, int p2, int p3);
	void HighlightItem(int index);
	void UnHighlight();
private:
	void EnableStatic(int i);
	void DisableStatic(int i);
	CUIStatic m_p[3];	
};