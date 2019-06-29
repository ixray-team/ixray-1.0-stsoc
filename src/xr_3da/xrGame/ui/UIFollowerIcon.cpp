#include "stdafx.h"
#include "UIFollowerIcon.h"
#include "../InventoryOwner.h"
#include "xrxmlparser.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "../level.h"
UIFollowerIcon::UIFollowerIcon()
{
	ZeroMemory			(m_icons,efiMax*sizeof(CUIStatic*));
}

UIFollowerIcon::~UIFollowerIcon()
{
	for(int i=0; i<efiMax;++i)
		if(m_icons[i])xr_delete(m_icons[i]);
}

void UIFollowerIcon::Init(Frect rect, const char* xml_name)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);
	inherited::Init(rect.x1, rect.y1, rect.width(), rect.height(), &uiXml);

	CUIStatic*	pItem = NULL;
	CUIXmlInit xml_init;

	if(uiXml.NavigateToNode("action_icon",0)){
		pItem = m_icons[efiUICurrentActionIcon] = xr_new<CUIStatic>();
		xml_init.InitStatic	(uiXml, "action_icon", 0, pItem);
		pItem->ClipperOn	();
		pItem->Show			(true);
		pItem->Enable		(true);
		AttachChild			(pItem);
	}

	if(uiXml.NavigateToNode("health_icon",0)){
		pItem = m_icons[efiUICurrentActionIcon] = xr_new<CUIStatic>();
		xml_init.InitStatic	(uiXml, "health_icon", 0, pItem);
		pItem->ClipperOn	();
		pItem->Show			(true);
		pItem->Enable		(true);
		AttachChild			(pItem);
	}
}

void UIFollowerIcon::InitCharacter	(u16 id)
{
//	CInventoryOwner* IO = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(id));
	inherited::InitCharacter(id);
}

void UIFollowerIcon::Draw()
{
	inherited::Draw();
}

void UIFollowerIcon::Update()
{
	inherited::Update();
}
