//=============================================================================
//  Filename:   UIDragDropItemMP.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Визуальная репрезентация вещи в меню покупки оружия в мультиплейере.
//	От синглплейерной ее отличает множество дополнительных свойств
//=============================================================================

#include "stdafx.h"
#include "UIDragDropItemMP.h"
#include "UIBuyWeaponWnd.h"
#include "../HUDManager.h"
#include "Restrictions.h"

extern CRestrictions g_mp_restrictions;

bool MP_item_cmp(CUIDragDropItem* i1, CUIDragDropItem* i2){
	CUIDragDropItemMP* _i1 = smart_cast<CUIDragDropItemMP*>(i1);
	CUIDragDropItemMP* _i2 = smart_cast<CUIDragDropItemMP*>(i2);

	if (_i1->GetPosInSubSection() > _i2->GetPosInSubSection())
		return false;
	else if (_i1->GetPosInSubSection() < _i2->GetPosInSubSection())
		return true;
	else
	{
		R_ASSERT2(false,"MP_item_cmp(CUIDragDropItem* i1, CUIDragDropItem* i1): pos1 = pos2");
		return true;
	}

}

//////////////////////////////////////////////////////////////////////////
CUIDragDropItemMP::CUIDragDropItemMP()
	:slotNum					(0),
	sectionNum					(0),
	bAddonsAvailable			(false),
	cost						(0),
	m_bAlreadyPaid				(false),
	m_bHasRealRepresentation	(false)
{
	std::strcpy(m_strAddonTypeNames[0], "Silencer");
	std::strcpy(m_strAddonTypeNames[1], "Grenade Launcher");
	std::strcpy(m_strAddonTypeNames[2], "Scope");

	for (u8 i = 0; i < NUM_OF_ADDONS; ++i)
		m_pAddon[i] = NULL;
}

//bool CUIDragDropItemMP::HasAmountControl(){
//	return g_mp_restrictions.HasAmountControl(GetSectionName());
//}
//
//bool CUIDragDropItemMP::GetPermissionToBuy(bool& last){
//	return g_mp_restrictions.GetPermission(GetSectionName(), last);
//}
//
//void CUIDragDropItemMP::OnReturn(){
//	g_mp_restrictions.Return(GetSectionName());
//}

void CUIDragDropItemMP::AttachDetachAddon(AddonIDs iAddonIndex, bool bAttach, bool bRealRepresentationSet)
{
	R_ASSERT(iAddonIndex >= 0 && iAddonIndex < 3);
	if (m_AddonInfo[iAddonIndex].iAttachStatus != -1)
	{
		CUIBag *this_inventory = smart_cast<CUIBag*>(GetOwner()->GetParent());
		R_ASSERT(this_inventory);

		CUIDragDropItemMP *pPossibleAddon = this_inventory->GetAddonByID(this, iAddonIndex);
        
		if (bAttach)
			R_ASSERT(this_inventory->IsItemInBag(pPossibleAddon));
		
		AttachDetachAddon(pPossibleAddon, bAttach, bRealRepresentationSet);
	}
}

void CUIDragDropItemMP::AttachDetachAddon(CUIDragDropItemMP *pPossibleAddon, bool bAttach, bool bRealRepresentationSet)
{	
	// Проверяем является ли pPossibleAddon действительно нашим аддоном
	AddonIDs ID = IsOurAddon(pPossibleAddon);
	if (ID != ID_NONE)
	{
		CUIBuyWeaponWnd *this_inventory = smart_cast<CUIBuyWeaponWnd*>(GetOwner()->GetMessageTarget());
		R_ASSERT(this_inventory);

		if (bAttach)
		{
			if (!pPossibleAddon->IsDragDropEnabled())
				return;
			if (m_AddonInfo[ID].iAttachStatus != 1 && pPossibleAddon->GetCost() <= this_inventory->GetMoneyAmount())
			{
				m_pAddon[ID] = pPossibleAddon;
				m_pAddon[ID]->GetParent()->DetachChild(pPossibleAddon);
							  GetParent()->AttachChild(pPossibleAddon);
				m_pAddon[ID]->Show(false);
				m_pAddon[ID]->EnableDragDrop(false);
				m_pAddon[ID]->Enable(false);
				m_pAddon[ID]->m_bHasRealRepresentation = bRealRepresentationSet;
				m_pAddon[ID]->Rescale(  smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleX(),
										smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleY());

				// Отнимаем денежку
//				this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
//					static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
				this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(-static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1))));
				m_pAddon[ID]->m_bAlreadyPaid = true;
			}
		}
		else
		{
			if (m_AddonInfo[ID].iAttachStatus != 0)
			{
				GetParent()->DetachChild(m_pAddon[ID]);
				m_pAddon[ID]->GetOwner()->AttachChild(m_pAddon[ID]);
				m_pAddon[ID]->Show(true);
				m_pAddon[ID]->EnableDragDrop(true);
				m_pAddon[ID]->Enable(true);
				m_pAddon[ID]->Rescale(  smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleX(),
										smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleY());
				// Прибавляем денежку
				if (m_pAddon[ID]->GetColor() != cUnableToBuy)
				{
//					this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() + 
//						static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
					this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1))));
					m_pAddon[ID]->m_bAlreadyPaid = true;
				}

				m_pAddon[ID] = NULL;
			}
		}
	}

	m_AddonInfo[ID].iAttachStatus = bAttach ? 1 : 0;
}

LPCSTR CUIDragDropItemMP::GetGranadesSectionName(){
	LPCSTR itemsList; 
	static string256 single_item;

	itemsList = pSettings->r_string(GetSectionName(), "grenade_class");
	_GetItem(itemsList, 0, single_item);

	return single_item;
}
//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::AttachDetachAllAddons(bool bAttach)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pAddon[i])
			AttachDetachAddon(static_cast<AddonIDs>(i), bAttach, m_pAddon[i]->m_bHasRealRepresentation);
	}
}

void CUIDragDropItemMP::Draw()
{
	inherited::Draw();

	if (m_bInFloat) 
		UI()->PushScissor(UI()->ScreenRect(),true);

	Frect rect = GetAbsoluteRect();

	//отцентрировать родительскую иконку по центру ее окна
	float right_offset = (GetWidth()-m_UIStaticItem.GetOriginalRectScaled().width())/2;
	float down_offset = (GetHeight()-m_UIStaticItem.GetOriginalRectScaled().height())/2;

	// Отрисовываем аддоны по порядку
	if (bAddonsAvailable)
	{
		CUIDragDropItemMP		*pDDItemMP = NULL;
		AddonInfo				nfo;
		for (int i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
		{
			if(0 == m_AddonInfo[i].iAttachStatus || !m_pAddon[i])
				continue;

			pDDItemMP	= m_pAddon[i];
			nfo			= m_AddonInfo[i];
///.
			pDDItemMP->GetUIStaticItem().SetPos(rect.left + right_offset + iFloor(0.5f+(float)nfo.x),
				rect.top + down_offset + iFloor(0.5f+(float)nfo.y));

//			pDDItemMP->Rescale(GetTextureScaleX(), GetTextureScaleY());
			pDDItemMP->GetUIStaticItem().Render();
		}
	}

	if (m_bInFloat) 
		UI()->PopScissor();
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP::AddonIDs CUIDragDropItemMP::IsOurAddon(CUIDragDropItemMP * pPossibleAddon)
{
	if (!pPossibleAddon) return ID_NONE;

	for (int i = 0; i < NUM_OF_ADDONS; ++i)
	{
		if (m_AddonInfo[i].iAttachStatus != -1								&&
			pPossibleAddon->GetSectionName() == m_AddonInfo[i].strAddonName)
		{
			return static_cast<AddonIDs>(i);
		}
	}

	return ID_NONE;
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::SetSlot(int slot)
{
	R_ASSERT(slot < MP_SLOTS_NUM || slot == static_cast<u32>(-1) || slot == WEAPON_BOXES_SLOT);
	slotNum = slot; 
}

void WpnDrawIndex(CUIDragDropItem *pDDItem)
{
	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pDDItem);
	R_ASSERT(pDDItemMP);
	if (!pDDItemMP) return;
	if (pDDItemMP->GetSectionGroupID() == GROUP_DEFAULT)
		return;

	if (strstr(pDDItemMP->GetSectionName(), "ammo") && pSettings->r_s32(pDDItemMP->GetSectionName(), "box_size")>1)
	{
		float left	= pDDItemMP->GetUIStaticItem().GetPosX();
		float bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();

		pDDItemMP->GetFont()->SetColor(0xff00ff00);
		UI()->OutText(pDDItem->GetFont(), pDDItemMP->GetSelfClipRect(), left, 
			bottom - pDDItemMP->GetFont()->CurrentHeight(),
			"%d", pSettings->r_s32(pDDItemMP->GetSectionName(), "box_size"));

	}
	else
	{

		float left	= pDDItemMP->GetUIStaticItem().GetPosX();
		float bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();

		pDDItemMP->GetFont()->SetColor(pDDItemMP->GetColor());
		UI()->OutText(pDDItem->GetFont(), pDDItemMP->GetSelfClipRect(), left, 
			bottom - pDDItemMP->GetFont()->CurrentHeightRel(),
			"%d", (pDDItemMP->GetPosInSubSection() + 1)%10);
	}

	pDDItemMP->GetFont()->OnRender();
}