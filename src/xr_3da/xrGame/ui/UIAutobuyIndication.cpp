// File:		UIAutobuyIndication.cpp
// Description:	Indication for 3 presets of autobuy command. for UIBuyWeaponWnd
// Created:		21.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua


// Copyright 2005 GSC Game World

#include "StdAfx.h"

#include "UIAutobuyIndication.h"
#include "UIInventoryUtilities.h"

using namespace InventoryUtilities;

const float HEIGHT = 15.0f;
const float WIDTH = 40.0f;
const float INDENT = 3.0f;
const u32 COLOR_HIGHLIGHTED = 0xffffffff;
const u32 COLOR_NORMAL      = 0xaaaaaaaa;
const u32 COLOR_DISABLED    = 0x0;

CUIAutobuyIndication::CUIAutobuyIndication(){
	AttachChild(&m_p[0]);
	AttachChild(&m_p[1]);
	AttachChild(&m_p[2]);
}

CUIAutobuyIndication::~CUIAutobuyIndication(){

}

void CUIAutobuyIndication::Init(float x, float y, float width, float height){
	CUIWindow::Init(x, y, WIDTH*3 + INDENT*2, HEIGHT);

	float _x = 0;

	for (int i = 0; i < 3; i++)
	{
		m_p[i].Init(_x + INDENT, 0, WIDTH, HEIGHT);
		m_p[i].SetShader(GetBuyMenuShader());
		m_p[i].SetOriginalRect(0, 1010, 39, 14);
		m_p[i].TextureOff();
		m_p[i].SetTextColor(COLOR_DISABLED);		
		_x += WIDTH + INDENT;
	}
}

void CUIAutobuyIndication::HighlightItem(int index){
	for (int i = 0; i < 3; i++)
		if (i == index)
		{
			m_p[i].TextureOn();
			m_p[i].SetTextColor(COLOR_HIGHLIGHTED);
		}
		else
		{
			m_p[i].TextureOff();
			m_p[i].SetTextColor(COLOR_NORMAL);
		}	
}

void CUIAutobuyIndication::UnHighlight(){
	for (int i = 0; i < 3; i++)
	{
		m_p[i].SetTextColor(COLOR_DISABLED);
		m_p[i].TextureOff();
	}
}

void CUIAutobuyIndication::SetPrices(int p1, int p2, int p3){
	char buf[5];

	m_p[0].SetText(itoa(p1, buf, 10));
	m_p[1].SetText(itoa(p2, buf, 10));
	m_p[2].SetText(itoa(p3, buf, 10));
}

