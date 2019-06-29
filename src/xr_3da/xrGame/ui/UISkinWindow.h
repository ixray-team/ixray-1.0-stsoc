// file:		UISkinWindow
// description:	SkinWindow for SkinSelector
// created:		03.02.2005
// author:		Serge Vynnychenko

// copyright 2005 GSC Game World

#pragma once

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UIXmlInit.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

const char * const	PATH_XML			= "frame_window";
const int			SKINS_COUNT			= 4;
const u32			clActive			= 0xffffffff;
const u32			clInactive			= 0xaa888888;

class CUISkinWindow : public CUIWindow{
public:
	CUISkinWindow();
	~CUISkinWindow();
    CUIStatic		UIHighlight;
	CUIFrameWindow	UIBackground;
    
	virtual void Init(CUIXml& xmlDoc, int index, shared_str section);
	virtual bool OnDbClick();
private:
	shared_str GetSkinName(shared_str section, int index);
};