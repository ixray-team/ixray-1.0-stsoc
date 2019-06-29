//=============================================================================
//  Filename:   UIContracts.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Contracts UI Window
//=============================================================================

#include "StdAfx.h"
#include "UIContracts.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

const char * const	CONTRACTS_XML			= "contracts.xml";
const char * const	CONTRACTS_CHAR_XML		= "contracts_character.xml";


//////////////////////////////////////////////////////////////////////////

CUIContractsWnd::CUIContractsWnd()
{
	SetWindowName("");
}

//////////////////////////////////////////////////////////////////////////

void CUIContractsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, CONTRACTS_XML);
	R_ASSERT3(xml_result, "xml file not found", CONTRACTS_XML);

	CUIXmlInit xml_init;

	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.EnableScrollBar(true);

	CUIWindow wnd;
	xml_init.InitWindow(uiXml, "window", 0, &wnd);

	Frect r = wnd.GetWndRect();
	AttachChild(&UICharInfo);
	UICharInfo.Init(r.left, r.top, r.right, r.bottom, CONTRACTS_CHAR_XML);
//	UICharInfo.m_bInfoAutoAdjust = false;

	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
}

//////////////////////////////////////////////////////////////////////////

void CUIContractsWnd::Show(bool status)
{
	inherited::Show(status);
	status ? UICharInfo.UIIcon().SetMask(&UIMask) : UICharInfo.UIIcon().SetMask(NULL);
}
