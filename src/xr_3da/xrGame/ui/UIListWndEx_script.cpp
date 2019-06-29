// File:        UIListWndEx_script.cpp
// Description: Exports to script
//              ListWndEx, ListItemEx, ListItemPair
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "stdafx.h"
#include "UIListWndEx.h"
#include "UIListItemPair.h"
#include "../script_space.h"

using namespace luabind;

//bool CUIListWndEx::AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont,bool doParse)
//{
//	CUIString			s;
//	s.SetText(str);
//	if(doParse){
//		CUIStatic::PreprocessText(s.m_str,GetWidth()-shift-5,pFont);
//	}
//	float shift_ = float(shift);
//	return AddParsedItem<CUIListItemEx>(s, shift_, color, pFont);
//
//}

#pragma optimize("s",on)
void CUIListWndEx::script_register(lua_State *L)
{

	module(L)
		[
//			class_<CUIListWndEx, CUIListWnd>("CUIListWndEx")
//			.def(							constructor<>())
//			.def("AddText",					&CUIListWndEx::AddText_script)
//			.def("GetExItem",				&CUIListWndEx::GetExItem),
			
			class_<CUIListItemEx, CUIListItem>("CUIListItemEx")
			.def(							constructor<>())
			.def("SetSelectionColor",		&CUIListItemEx::SetSelectionColor),
//			.def("SetPerformTextLimit",		&CUIListItemPair::SetPerformTextLimit),


			class_<CUIListItemPair, CUIListItemEx>("CUIListItemPair")
			.def(							constructor<>())
			.def("SetTextFileName",			&CUIListItemPair::SetTextFileName)
			.def("GetTextFileName",			&CUIListItemPair::GetTextFileName)
			.def("SetTextDateTime",			&CUIListItemPair::SetTextDateTime)
			.def("GetTextDateTime",			&CUIListItemPair::GetTextDateTime)
			.def("SetFontFileName",			&CUIListItemPair::SetFontFileName)
			.def("SetFontDateTime",			&CUIListItemPair::SetFontDateTime)
			.def("SetTextFileName",			&CUIListItemPair::SetTextFileName)
			.def("GetBorder",				&CUIListItemPair::GetBorder)
			.def("SetBorder",				&CUIListItemPair::SetBorder)		

		];
}