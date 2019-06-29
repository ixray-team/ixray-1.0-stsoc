// File:        UIListItemPair.cpp
// Description: This list item designed special for CUIListWndEx to represent
//				two properties of save file - name and date.
//              ______________________________
//             | file_name       |  date/time |
//             |_________________|____________|
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "StdAfx.h"
#include "UIListItemPair.h"
#include "../HUDManager.h"
#include "../UI.h"

//-----------------------
// Construction
//-----------------------
CUIListItemPair::CUIListItemPair(){
	// set textures
	this->InitTexture("ui\\hud_map_point");
	this->SetStretchTexture(true);
	this->m_dwSelectionColor = color_argb(200, 95, 82, 74);
	this->SetColor(color_argb(0, 0, 0, 0));

	// attach all children
	this->AttachChild(&m_staticFileName);
	this->AttachChild(&m_staticDateTime);

    m_iBorder = 60;

	// file name field
	m_staticFileName.SetFont(UI()->Font()->pFontMedium);
	m_staticFileName.SetTextColor(color_argb(255, 240, 217, 182));
	m_staticFileName.SetTextAlignment(CGameFont::alLeft);
	m_staticFileName.Init(0,0,0,0);
	m_staticFileName.SetElipsis(CUIStatic::eepNone, 1);

	// date/time field
	m_staticDateTime.SetFont(UI()->Font()->pFontMedium);
	m_staticDateTime.SetTextColor(color_argb(255, 216, 186, 140));
	m_staticDateTime.SetTextAlignment(CGameFont::alLeft);
	m_staticDateTime.Init(0,0,0,0);
}

//-----------------------
// Destruction
//-----------------------
CUIListItemPair::~CUIListItemPair(){

}

//-------------------------------------------------

void CUIListItemPair::Draw(){	
	//CUIListItemEx::Draw();
	Frect			rect;
	GetAbsoluteRect	(rect);

	if(m_bAvailableTexture && m_bTextureEnable){		
		m_UIStaticItem.SetPos(rect.left, rect.top);		
#pragma todo("ALEXMX -> SATAN")
//.		if(m_bStretchTexture)
//.			//растягиваем текстуру, Clipper в таком случае игнорируется (пока)
//.			m_UIStaticItem.Render(0, 0, rect.right-rect.left, rect.bottom-rect.top);
//.		else
			m_UIStaticItem.Render();
	}

	float width  = this->GetWidth();
	float height = this->GetHeight();
	const int indent = 10; // indent between 
	
	// get max length for both elements
	float maxFileNameLength = width * this->m_iBorder/100 - indent;
	float maxDateTimeLength = width - maxFileNameLength;

	// get length of elements
//	int realFileNameLength = (int)m_staticFileName.GetFont()->SizeOf(m_staticFileName.GetText());
//	int realDateTimeLength = (int)m_staticDateTime.GetFont()->SizeOf(m_staticDateTime.GetText());

	// get height of elements
	float fileNameHeight = m_staticFileName.GetFont()->CurrentHeight_();
	float dateTimeHeight = m_staticDateTime.GetFont()->CurrentHeight_();

	//m_staticFileName.Init
	m_staticFileName.SetWndRect(0,                          (height - fileNameHeight)/2 , maxFileNameLength, height);
	//m_staticDateTime.Init
	m_staticDateTime.SetWndRect(maxFileNameLength + indent, (height - dateTimeHeight)/2 , maxDateTimeLength, height);

	m_staticFileName.Draw();
	m_staticDateTime.Draw();
}

void CUIListItemPair::SetHighlightText(bool Highlight){
	//m_staticFileName.SetHighlightText(Highlight); 
	//m_staticDateTime.SetHighlightText(Highlight); 
}

//-------------------------------------------------

void CUIListItemPair::SetText(LPCSTR file_name, LPCSTR data_time){
	m_staticFileName.SetText(file_name);	
	m_staticDateTime.SetText(data_time);
	
}

//-------------------------------------------------

void CUIListItemPair::SetTextFileName(LPCSTR file_name){
	m_staticFileName.SetText(file_name);	
	CUIListItem::SetText(file_name);
}

//-------------------------------------------------

void CUIListItemPair::SetTextDateTime(LPCSTR date_time){
	m_staticDateTime.SetText(date_time);	
}

//-------------------------------------------------

void CUIListItemPair::SetFontFileName(CGameFont* pFont){
	m_staticFileName.SetFont(pFont);	
}

//-------------------------------------------------

void CUIListItemPair::SetFontDateTime(CGameFont* pFont){
	m_staticDateTime.SetFont(pFont);
}

//-------------------------------------------------

int CUIListItemPair::GetBorder(){	
	return m_iBorder;	
}

//---->>

void CUIListItemPair::SetBorder(int iBorder){
	// border = [1, 100]
	m_iBorder = iBorder%100;
}

LPCSTR CUIListItemPair::GetTextFileName(){
	return this->m_staticFileName.GetText();
}

LPCSTR CUIListItemPair::GetTextDateTime(){
	return this->m_staticDateTime.GetText();
}

LPCSTR CUIListItemPair::GetText(){
	return this->m_staticFileName.GetText();	
}

void CUIListItemPair::SetTextColor(u32 color){
	m_staticFileName.SetTextColor(color);
	m_staticDateTime.SetTextColor(color);
}