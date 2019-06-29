// UIBuyWeaponWnd.cpp:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
#include "UIBuyWeaponWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "UISkinSelector.h"
#include "../uigamesp.h"
#include "../level.h"
#include "UIInventoryUtilities.h"
#include "../InfoPortion.h"
#include "../game_cl_Deathmatch.h"
#include "../string_table.h"
#include "../actor.h"
#include "../inventory.h"
#include "UITextureMaster.h"
#include "UIScrollView.h"
#include "UIListBoxItem.h"


using namespace InventoryUtilities;

int			g_iOkAccelerator, g_iCancelAccelerator;


CUIBuyWeaponWnd::CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection)
		: UIBagWnd(BagProc)
{
	m_StrSectionName		= strSectionName;
	Hide					();

	m_pCurrentDragDropItem	= NULL;
//	m_iMoneyAmount			= 10000;

	SetFont					(HUD().Font().pFontMedium);
	
	m_bIgnoreMoneyAndRank	= false;

	// Инициализируем вещи
	Init					(strSectionName, strPricesSection);

	Hide					();
}

CUIBuyWeaponWnd::~CUIBuyWeaponWnd()
{
}

void CUIBuyWeaponWnd::Init(LPCSTR strSectionName, LPCSTR strPricesSection)
{
	CUIXml uiXml;
	CStringTable string_table;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, BUY_WND_XML_NAME);
	R_ASSERT2(xml_result, "xml file not found");

	inherited::DetachAll();

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_WIDTH);

	AttachChild(&UIBackground);
	UIBackground.Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
	UIBackground.SetShader(GetBuyMenuShader());	

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	UIBagWnd.Init(uiXml,"bag_background_static", strSectionName, strPricesSection);

	////////////////////////////////////////
	// окно с описанием активной вещи
	AttachChild(&UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &UIDescWnd);

	////////////////////////////////////
	//Окно с информации о персонаже
	AttachChild(&UIPersonalWnd);
	xml_init.InitStatic(uiXml, "personal_static", 0, &UIPersonalWnd);

	// Tabcontrol для оружия
	AttachChild(&UITabControl);
	UITabControl.Init(&uiXml, "tab");

	BringToTop(&UITabControl);	

	// Кнопки OK и Cancel для выходи из диалога покупки оружия
	AttachChild(&UIBtnOK);
	xml_init.Init3tButton(uiXml, "ok_button", 0, &UIBtnOK);
	UIBtnOK.EnableTextHighlighting(false);
	g_iOkAccelerator = uiXml.ReadAttribInt("ok_button", 0, "accel");

	AttachChild(&UIBtnCancel);
	xml_init.Init3tButton(uiXml, "cancel_button", 0, &UIBtnCancel);
	UIBtnCancel.EnableTextHighlighting(false);
	g_iCancelAccelerator = uiXml.ReadAttribInt("cancel_button", 0, "accel");

	AttachChild(&UIBtnClear);
	xml_init.Init3tButton(uiXml, "clear_button", 0, &UIBtnClear);
	UIBtnClear.EnableTextHighlighting(false);

//	AttachChild(&UIBtn_BulletPistol);
//	xml_init.Init3tButton(uiXml, "btn_bullets_pistol", 0, &UIBtn_BulletPistol);

//	AttachChild(&UIBtn_BulletRiffle);
//	xml_init.Init3tButton(uiXml, "btn_bullets_riffle", 0, &UIBtn_BulletRiffle);

	UIBagWnd.UpdateBuyPossibility();

	//Списки Drag&Drop
	AttachChild(&UITopList[BELT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UITopList[BELT_SLOT]);

	AttachChild(&UITopList[OUTFIT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UITopList[OUTFIT_SLOT]);

	AttachChild(&UITopList[0]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UITopList[0]);
	//-----------------------------------------------------
	//чтобы нельзя было продать нож
	UITopList[0].BlockCustomPlacement();
	UITopList[0].EnableDoubleClick(false);
	UITopList[0].Enable(false);
	//-----------------------------------------------------

	AttachChild(&UITopList[1]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UITopList[1]);
	UITopList[1].BlockCustomPlacement();

	AttachChild(&UITopList[2]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 4, &UITopList[2]);
	UITopList[2].BlockCustomPlacement();

	AttachChild(&UITopList[3]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 5, &UITopList[3]);
	UITopList[3].BlockCustomPlacement();

	AttachChild(&UITopList[4]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 6, &UITopList[4]);
	UITopList[4].BlockCustomPlacement();

	UITopList[KNIFE_SLOT].SetCheckProc(SlotProc0);
	UITopList[PISTOL_SLOT].SetCheckProc(SlotProc1);
	UITopList[RIFLE_SLOT].SetCheckProc(SlotProc2);
	UITopList[GRENADE_SLOT].SetCheckProc(SlotProc3);
	UITopList[APPARATUS_SLOT].SetCheckProc(SlotProc4);
	UITopList[OUTFIT_SLOT].SetCheckProc(OutfitSlotProc);
	UITopList[BELT_SLOT].SetCheckProc(BeltProc);

	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init(0,0,300,300);
	UIPropertiesBox.Hide();

	// Статики для вывода текста
	AttachChild(&UITotalMoneyHeader);
	xml_init.InitStatic(uiXml, "total_money_header_static", 0, &UITotalMoneyHeader);

	AttachChild(&UITotalMoney);
	xml_init.InitStatic(uiXml, "total_money_static", 0, &UITotalMoney);

	AttachChild(&UIItemCostHeader);
	xml_init.InitStatic(uiXml, "item_cost_header_static", 0, &UIItemCostHeader);

	AttachChild(&UIItemCost);
	xml_init.InitStatic(uiXml, "item_cost_static", 0, &UIItemCost);

	// Иконка изображения персонажа в костюме
	AttachChild(&UIOutfitIcon);
	xml_init.InitStatic(uiXml, "outfit_static", 0, &UIOutfitIcon);
//	UIOutfitIcon.SetShader(GetMPCharIconsShader());
	UIOutfitIcon.SetStretchTexture(true);
//	UIOutfitIcon.ClipperOn();

	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), BUY_MP_ITEM_XML);
	UIDescWnd.AttachChild(&UIDescRankIcon);
	xml_init.InitStatic(uiXml, "desc_static:rank_icon",0,&UIDescRankIcon);

	AttachChild(&UIBtn_PistolSilencer);
	CUIXmlInit::Init3tButton(uiXml, "btn_pistol_silencer", 0, &UIBtn_PistolSilencer);

	AttachChild(&UIBtn_PistolBullet);
	CUIXmlInit::Init3tButton(uiXml, "btn_pistol_bullets", 0, &UIBtn_PistolBullet);

	AttachChild(&UIBtn_RifleScope);
	CUIXmlInit::Init3tButton(uiXml, "btn_rifle_scope", 0, &UIBtn_RifleScope);

	AttachChild(&UIBtn_RifleSilencer);
	CUIXmlInit::Init3tButton(uiXml, "btn_rifle_silencer", 0, &UIBtn_RifleSilencer);

	AttachChild(&UIBtn_RifleGranadelauncer);
	CUIXmlInit::Init3tButton(uiXml, "btn_riffle_granadelauncher", 0, &UIBtn_RifleGranadelauncer);

	AttachChild(&UIBtn_RifleGranade);
	CUIXmlInit::Init3tButton(uiXml, "btn_riffle_granades", 0, &UIBtn_RifleGranade);

	AttachChild(&UIBtn_RifleBullet);
	CUIXmlInit::Init3tButton(uiXml, "btn_rifle_bullets", 0, &UIBtn_RifleBullet);

	// disable
	if (GameID() == GAME_DEATHMATCH)
	{
		UIBtn_PistolBullet.Enable(false);
		UIBtn_PistolBullet.SetVisible(false);
		UIBtn_RifleBullet.Enable(false);
		UIBtn_RifleBullet.SetVisible(false);
	}
	UIBtn_PistolSilencer.Enable(false);
	UIBtn_PistolSilencer.SetVisible(false);
	UIBtn_RifleSilencer.Enable(false);
	UIBtn_RifleSilencer.SetVisible(false);
	UIBtn_RifleScope.Enable(false);
	UIBtn_RifleScope.SetVisible(false);
	UIBtn_RifleGranadelauncer.Enable(false);
	UIBtn_RifleGranadelauncer.SetVisible(false);
}

void CUIBuyWeaponWnd::OnMouseScroll(float iDirection){

}


void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
}  

//проверка на помещение инвентаря в слоты
// KNIFE SLOT
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) 
		return false;

	//bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last)){
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;
	this_inventory->SlotToSection(KNIFE_SLOT);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(-GetItemPrice(pDDItemMP)));
	}
	return true;
}

void CUIBuyWeaponWnd::HighlightCurAmmo(){
	CUIDragDropItem *_slot1 = NULL;
	CUIDragDropItem *_slot2 = NULL;

	if (!UITopList[PISTOL_SLOT].GetDragDropItemsList().empty())
		_slot1 = UITopList[PISTOL_SLOT].GetDragDropItemsList().front(); 

	if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
        _slot2 = UITopList[RIFLE_SLOT].GetDragDropItemsList().front(); 

	CUIDragDropItemMP *slot1 = NULL;
	CUIDragDropItemMP *slot2 = NULL;

	if (_slot1)
		slot1 = smart_cast<CUIDragDropItemMP*>(_slot1);

	if (_slot2)
		slot2 = smart_cast<CUIDragDropItemMP*>(_slot2);

	UIBagWnd.HighLightAmmo(slot1? slot1->GetSectionName() : NULL, slot2 ? slot2->GetSectionName(): NULL);
}

// PISTOL SLOT
bool CUIBuyWeaponWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

//	bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last)){
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;
	this_inventory->SlotToSection(PISTOL_SLOT);
	this_inventory->HighlightCurAmmo();

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(- GetItemPrice(pDDItemMP)));
	}
	return true;
}

// RIFLE_SLOT
bool CUIBuyWeaponWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP::AddonIDs	addonID;
	CUIDragDropItemMP *pDDItemMP	= smart_cast<CUIDragDropItemMP*>(pItem), 
					  *pAddonOwner	= this_inventory->IsItemAnAddon(pDDItemMP, addonID);

	R_ASSERT(pDDItemMP);

	// Если аддон
	if (pAddonOwner)	
	{
		pAddonOwner->AttachDetachAddon(pDDItemMP, !pAddonOwner->IsAddonAttached(addonID), pDDItemMP->m_bHasRealRepresentation);
		return false;
	}
	// Не аддон
	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) 
		return false;

//	bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last)){
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;
	this_inventory->SlotToSection(RIFLE_SLOT);
	this_inventory->HighlightCurAmmo();

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;		
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(- GetItemPrice(pDDItemMP)));
	}

	return true;
}


// GRENADE_SLOT
bool CUIBuyWeaponWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// Если вещь не граната
	// и если в слоте уже есть 1 граната
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

//	bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last)){
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(- GetItemPrice(pDDItemMP)));
	}

	return true;
}

//APPARATUS_SLOT
bool CUIBuyWeaponWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) 
		return false;

//	bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last)){
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(- GetItemPrice(pDDItemMP)));
	}

	return true;
}

//одеть костюм
bool CUIBuyWeaponWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		this_inventory->SlotToSection(OUTFIT_SLOT);
		pDDItemMP->Show(false);

		R_ASSERT(!pDDItemMP->m_additionalInfo.empty());

		this_inventory->UIOutfitIcon.InitTexture(pDDItemMP->m_additionalInfo.c_str());
		this_inventory->UIOutfitIcon.Show(true);
		this_inventory->UIOutfitIcon.SetColor(pDDItemMP->GetColor());

		// И отнимаем от денег стоимость вещи.
		if (!pDDItemMP->m_bAlreadyPaid)
		{
//			this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
			pDDItemMP->m_bAlreadyPaid = true;
			this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft( - GetItemPrice(pDDItemMP)));
		}
//		this_inventory->UpdateOutfit();
		return true;
	}	
	return false;
}

//на пояс
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);

	CUIDragDropItemMP::AddonIDs tmp;

	// Если вещь - аддон, то никуда ее не суем
	if (!this_inventory->CanPutInBelt(pDDItemMP)		||
		this_inventory->IsItemAnAddon(pDDItemMP, tmp)) 
		return false;

//	bool last;
	//if (pDDItemMP->HasAmountControl())
	//	if (pDDItemMP->GetPermissionToBuy(last))
	//	{
	//		if (!last)
	//			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	//	}
	//	else
	//		return false;
	
	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(- GetItemPrice(pDDItemMP)));
		UNHIGHTLIGHT_ITEM(pDDItemMP);
	}

	return true;
}

void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	TABS_VECTOR_it	it;

	switch (msg)
	{
	case TAB_CHANGED:
        OnTabChange();	break;

	case XR_MENU_LEVEL_CHANGED:
		OnMenuLevelChange(); break;

	case DRAG_DROP_ITEM_DB_CLICK:
		SetCurrentDDItem(pWnd);
		OnDDItemDbClick(); break;

	case BUTTON_CLICKED:
		if (&UIBtnOK == pWnd)
			OnBtnOkClicked();
		else if (&UIBtnCancel == pWnd)
			OnBtnCancelClicked();
		else if (&UIBtnClear == pWnd)
			OnBtnClearClicked();
		else if (&UIBtn_PistolBullet == pWnd)
			OnBtnBulletBuy(PISTOL_SLOT);
		else if (&UIBtn_RifleBullet == pWnd)
			OnBtnBulletBuy(RIFLE_SLOT);
		else if (&UIBtn_PistolSilencer == pWnd)
			OnBtnSilencerBuy(PISTOL_SLOT);
		else if (&UIBtn_RifleSilencer == pWnd)
			OnBtnSilencerBuy(RIFLE_SLOT);
		else if (&UIBtn_RifleScope == pWnd)
			OnBtnRifleScope();
		else if (&UIBtn_RifleGranadelauncer == pWnd)
			OnBtnRifleGranadelauncher();
		else if (&UIBtn_RifleGranade == pWnd)			
			OnBtnRifleGranade();
		break;

	case DRAG_DROP_ITEM_DRAG:
		SetCurrentDDItem(pWnd);
		OnDDItemDrag(); break;

	case DRAG_DROP_REFRESH_ACTIVE_ITEM:
		if (m_pCurrentDragDropItem) 
			m_pCurrentDragDropItem->Highlight(true); break;

	case DRAG_DROP_ITEM_PLACED:
//			list = smart_cast<CUIDragDropList*>(pWnd);
//			if (UIBagWnd.IsChild(list))
//                list->SortList(MP_item_cmp);
		break;
	}

	//по нажатию правой кнопки
	if(msg == DRAG_DROP_ITEM_RBUTTON_CLICK)
	{
		SetCurrentDDItem(pWnd);
		ActivatePropertiesBox();
	}
	//сообщение от меню вызываемого правой кнопкой
	else if(pWnd == &UIPropertiesBox &&	msg == PROPERTY_CLICKED)
	{
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetID())
			{
			case BUY_ITEM_ACTION:
				SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
				break;
			case CANCEL_BUYING_ACTION:
				m_pCurrentDragDropItem->MoveOnNextDrop();
				UIBagWnd.SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);
				break;
			case ATTACH_SCOPE_ADDON:
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON:
//				bool bPistolAvailable = UITopList[PISTOL_SLOT].GetChildWndList().size() ? true : false;
//				bool bRiffleAvailable = UITopList[RIFLE_SLOT].GetChildWndList().size() ? true : false;
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON_PISTOL:
				SetCurrentDDItem(UITopList[PISTOL_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON_PISTOL:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			}
		}
	}


	// Кнопки ОК и Отмена
	// Если костюмчик вернулся в слот, то спрятать его
	if (OUTFIT_RETURNED_BACK == msg && pWnd->GetParent() == &UITopList[OUTFIT_SLOT])
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pWnd);
		if (pDDItemMP && OUTFIT_SLOT == pDDItemMP->GetSlot())
		{
			pDDItemMP->Show(false);
		}
	}
	
	if (WINDOW_LBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
		if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
			UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	
	//else if (WINDOW_RBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
	//{
	//	if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
	//	{
	//		UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	//		SendMessage(UITopList[OUTFIT_SLOT].GetChildWndList().back(), DRAG_DROP_ITEM_RBUTTON_CLICK, NULL);
	//	}
	//}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBuyWeaponWnd::OnTabChange(){
	UIBagWnd.ShowSection(UITabControl.GetActiveIndex());
}

void CUIBuyWeaponWnd::OnBtnOkClicked(){
	if (!CanBuyAllItems())
        return;

	Game().StartStopMenu(this,true);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	dm->OnBuyMenu_Ok();
}

void CUIBuyWeaponWnd::OnBtnCancelClicked(){
	Game().StartStopMenu(this,true);
}

void CUIBuyWeaponWnd::OnBtnClearClicked(){
	if (ClearTooExpensiveItems())
		return;

	SlotToSection(PISTOL_SLOT);
	SlotToSection(RIFLE_SLOT);
	SlotToSection(GRENADE_SLOT);
	SlotToSection(APPARATUS_SLOT);
	SlotToSection(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		BeltToSection(static_cast<CUIDragDropItemMP*>(*it));

	// put free pistol
//	GetTop()->SendMessage(UIBagWnd.GetItemByKey(DIK_1, GROUP_2), DRAG_DROP_ITEM_DB_CLICK);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	if (dm) dm->OnBuyMenu_DefaultItems();
}
#include "../../xr_input.h"

void CUIBuyWeaponWnd::OnBtnBulletBuy(int slot){
	if (UITopList[slot].GetDragDropItemsList().empty())
		return;

	CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[slot].GetDragDropItemsList().begin());

	R_ASSERT(pSettings->section_exist(item->GetSectionName()));

	xr_string itemsList; 
	string256 single_item;

	itemsList = pSettings->r_string(item->GetSectionName(), "ammo_class");
	int n = 0;
	if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
		n = 1;
	if (_GetItemCount(itemsList.c_str())<2)
		n = 0;
	_GetItem(itemsList.c_str(), n, single_item);
//	Msg("Ammo - %s", single_item);

	item = UIBagWnd.GetItemBySectoin(single_item);
	if (item)
        GetTop()->SendMessage(item, DRAG_DROP_ITEM_DB_CLICK, NULL);
}

void CUIBuyWeaponWnd::OnBtnSilencerBuy(int slot){
	R_ASSERT(!UITopList[slot].GetDragDropItemsList().empty());
		

	CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[slot].GetDragDropItemsList().begin());

	if (UIBagWnd.GetItemBySectoin(item->m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].strAddonName.c_str()))
	{
		item->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, 
			!item->IsAddonAttached(CUIDragDropItemMP::ID_SILENCER), 
			item->m_bHasRealRepresentation);
	}
	else if (item->IsAddonAttached(CUIDragDropItemMP::ID_SILENCER))
		item->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, item->m_bHasRealRepresentation);


}

void CUIBuyWeaponWnd::OnBtnRifleScope(){
	R_ASSERT(!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty());

	CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[RIFLE_SLOT].GetDragDropItemsList().begin());

	if (UIBagWnd.GetItemBySectoin(item->m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].strAddonName.c_str())){
//        GetTop()->SendMessage(item, DRAG_DROP_ITEM_DB_CLICK, NULL);
		item->AttachDetachAddon(	CUIDragDropItemMP::ID_SCOPE, 
			!item->IsAddonAttached(CUIDragDropItemMP::ID_SCOPE), 
			item->m_bHasRealRepresentation);
	}
	else if (item->IsAddonAttached(CUIDragDropItemMP::ID_SCOPE))
		item->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false, item->m_bHasRealRepresentation);

}

void CUIBuyWeaponWnd::OnBtnRifleGranadelauncher(){
	R_ASSERT(!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty());

	CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[RIFLE_SLOT].GetDragDropItemsList().begin());

	if (UIBagWnd.GetItemBySectoin(item->m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].strAddonName.c_str())){
//        GetTop()->SendMessage(item, DRAG_DROP_ITEM_DB_CLICK, NULL);
		item->AttachDetachAddon(	CUIDragDropItemMP::ID_GRENADE_LAUNCHER, 
			!item->IsAddonAttached(CUIDragDropItemMP::ID_GRENADE_LAUNCHER), 
			item->m_bHasRealRepresentation);
	}
	else if (item->IsAddonAttached(CUIDragDropItemMP::ID_GRENADE_LAUNCHER))
		item->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false, item->m_bHasRealRepresentation);
}

void CUIBuyWeaponWnd::OnBtnRifleGranade(){
	R_ASSERT(!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty());

	CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[RIFLE_SLOT].GetDragDropItemsList().begin());

	R_ASSERT(item->bAddonsAvailable);

	CUIDragDropItemMP* granade = UIBagWnd.GetItemBySectoin(item->GetGranadesSectionName());

	if (granade)
        GetTop()->SendMessage(granade, DRAG_DROP_ITEM_DB_CLICK, NULL);
}

void CUIBuyWeaponWnd::UpdateBuyButtons(){
	if (UITopList[PISTOL_SLOT].GetDragDropItemsList().empty())
	{
		UIBtn_PistolBullet.Enable(false);
		UIBtn_PistolSilencer.Enable(false);
	} else{
		CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[PISTOL_SLOT].GetDragDropItemsList().begin());
		UIBtn_PistolBullet.Enable(true);
		UIBtn_PistolSilencer.Enable(!item->m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].strAddonName.empty());
	}

	if (UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	{		
		UIBtn_RifleBullet.Enable(false);
		UIBtn_RifleSilencer.Enable(false);
		UIBtn_RifleScope.Enable(false);
		UIBtn_RifleGranadelauncer.Enable(false);
		UIBtn_RifleGranade.Enable(false);
	} else{
		CUIDragDropItemMP* item = smart_cast<CUIDragDropItemMP*>(*UITopList[RIFLE_SLOT].GetDragDropItemsList().begin());
		
		UIBtn_RifleBullet.Enable(true);
		UIBtn_RifleSilencer.Enable(!item->m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].strAddonName.empty());
		UIBtn_RifleScope.Enable(!item->m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].strAddonName.empty());
//		int status = pSettings->r_s32(item->GetSectionName(),"grenade_launcher_status");
		UIBtn_RifleGranadelauncer.Enable(!item->m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].strAddonName.empty());
		bool grenade = false;
		if (item->m_AddonInfo[1].iAttachStatus >= 1)
			grenade = true;		
		UIBtn_RifleGranade.Enable(grenade);
	}

	
    
}

bool CUIBuyWeaponWnd::ClearTooExpensiveItems(){
	bool f = false;
	bool res;

    res = ClearSlot_ifTooExpensive(PISTOL_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(RIFLE_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(GRENADE_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(APPARATUS_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(OUTFIT_SLOT);
	f = f ? true : res;

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		if ((*it)->GetColor() == cUnableToBuy)
		{
			GetTop()->SendMessage(*it, DRAG_DROP_ITEM_DB_CLICK);
			f = true;
		}

	return f;
}

bool CUIBuyWeaponWnd::ClearSlot_ifTooExpensive(int slot){
	bool empty;
	DRAG_DROP_LIST_it it;

	empty = UITopList[slot].GetDragDropItemsList().empty();

	if (!empty)
	{
		it = UITopList[slot].GetDragDropItemsList().begin();
		if ((*it)->GetColor() == cUnableToBuy)
		{
			GetTop()->SendMessage(*it, DRAG_DROP_ITEM_DB_CLICK);
			return true;
		}
	}

	return false;
}

void CUIBuyWeaponWnd::OnDDItemDrag(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
		m_pCurrentDragDropItem->Rescale(1.0f,1.0f);

	// Disable highliht in all DD lists
	for (int i = 0; i < PDA_SLOT; ++i)
		UITopList[i].HighlightAllCells(false);
}

void CUIBuyWeaponWnd::SetCurrentDDItem(CUIWindow* pWnd){
	if (m_pCurrentDragDropItem) 
		m_pCurrentDragDropItem->Highlight(false);

	m_pCurrentDragDropItem = smart_cast<CUIDragDropItemMP*>(pWnd);

#ifdef DEBUG
	R_ASSERT2(m_pCurrentDragDropItem, "CUIBuyWeaponWnd::SetCurrentDDItem - invalid item");
#endif
}

void CUIBuyWeaponWnd::OnDDItemDbClick(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
	{
		SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);

		if(!ToSlot())
			if(!ToBelt())
				if(!ToBag())
				//если нельзя, то просто упорядочить элемент в своем списке
				{
					CUIWindow* pWindowParent = m_pCurrentDragDropItem->GetParent();
					pWindowParent->DetachChild(m_pCurrentDragDropItem);
					pWindowParent->AttachChild(m_pCurrentDragDropItem);
					m_pCurrentDragDropItem->Rescale(	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleX(),
														((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleY()	);
				}

		HighlightCurAmmo();
	}
	CUIDragDropList* owner = smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetOwner());
	owner->SortList(MP_item_cmp);	
}

void CUIBuyWeaponWnd::OnMenuLevelChange(){
	if (UIBagWnd.GetMenuLevel() == mlRoot)
		UITabControl.SetActiveState();
}

bool CUIBuyWeaponWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DOWN == mouse_action || WINDOW_RBUTTON_DOWN == mouse_action)
	{
		Frect		r = UIOutfitIcon.GetAbsoluteRect();

		if ( r.in(x,y))
		{
			SendMessage(&UIOutfitIcon, static_cast<s16>(mouse_action), NULL);
		}
	}

	//вызов дополнительного меню по правой кнопке
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	return inherited::OnMouse(x, y, mouse_action);
}

void CUIBuyWeaponWnd::Draw()
{
	inherited::Draw();
}


void CUIBuyWeaponWnd::Update()
{
	UpdateBuyButtons();
	// Update money amount for Bag
//	UIBagWnd.UpdateMoney(m_iMoneyAmount);
	UIBagWnd.UpdateBuyPossibility();
	

	// Если активная вещь изменилась, то обновляем информацию о ее стоимости
	static CUIDragDropItemMP *pOldCurrentDragDropItem = reinterpret_cast<CUIDragDropItemMP*>(1);
	static string16		str;
	//m_pCurrentDragDropItem = UIBagWnd.GetCurrentItem();

	if (pOldCurrentDragDropItem != m_pCurrentDragDropItem && m_pCurrentDragDropItem)
	{
		pOldCurrentDragDropItem	= m_pCurrentDragDropItem;
		pOldCurrentDragDropItem->Highlight(false);

		UIItemCost.SetText("");
		FillItemInfo(NULL);

		if (m_pCurrentDragDropItem)
		{
			if (m_pCurrentDragDropItem->GetSlot() != WEAPON_BOXES_SLOT)
			{
				int cost = m_pCurrentDragDropItem->GetCost();
				if (m_pCurrentDragDropItem->m_bHasRealRepresentation)
					cost = static_cast<int>(cost * fRealItemSellMultiplier);
				sprintf(str, "%i", cost);
				UIItemCost.SetText(str);
				FillItemInfo(m_pCurrentDragDropItem);
			}
			m_pCurrentDragDropItem->Highlight(true);
		}
	}

	// Если деньги иземниличь то обновить и их
	static int oldMoneyAmount	= 0xffffffff;

	if (oldMoneyAmount != GetMoneyAmount())
	{
		oldMoneyAmount = GetMoneyAmount();
		sprintf(str, "%i", GetMoneyAmount());
		UITotalMoney.SetText(str);
	}

	// Ecли в слоте с костюмом армор показывается, то спрятать его.
	static bool flag = true;

	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
		flag = true;

	if (flag && UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
	{
		UpdateOutfit();
		flag = false;
	}

	CUIWindow::Update();
}

void CUIBuyWeaponWnd::UpdateOutfit(){
	UIOutfitIcon.InitTexture(m_current_skin.c_str());
	UIOutfitIcon.RescaleRelative2Rect(CUITextureMaster::GetTextureRect(m_current_skin.c_str()));
}

void CUIBuyWeaponWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);
	m_pCurrentDragDropItem = NULL;
}

void CUIBuyWeaponWnd::Show() 
{ 
	m_pMouseCapturer = NULL;
	inherited::Show();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;
		pActor->SetWeaponHideState(INV_STATE_BUY_MENU, true);
	}
	UITabControl.SetActiveState();
}

void CUIBuyWeaponWnd::Hide()
{
	inherited::Hide();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		pActor->SetWeaponHideState(INV_STATE_BUY_MENU, false);
	}
}

bool CUIBuyWeaponWnd::CanAttachAddOn(CUIDragDropItemMP* pAddonOwner, CUIDragDropItemMP* pAddOn){
	for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
		if (pAddonOwner->m_AddonInfo[j].strAddonName == pAddOn->GetSectionName())
			return true;

	return false;
}


// Контекстное меню
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	float x,y;
	Frect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	// Если обычная вещь
	if((m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM				||
		static_cast<u32>(-1) == m_pCurrentDragDropItem->GetSlot())		&& 
		&UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		// Если вещь не аддон, то действие одно, а если аддон, то другое
		CUIDragDropItemMP::AddonIDs		ID;
		CUIDragDropItemMP				*pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);

		if (!HasEnoughMoney(m_pCurrentDragDropItem)/*m_pCurrentDragDropItem->GetCost() > GetMoneyAmount()*/)
		{
			return;
		}
		
		if (pDDItemMP)
		{
			
			bool bPistolAvailable = UITopList[PISTOL_SLOT].GetDragDropItemsList().size() ? true : false;
			bool bRiffleAvailable = UITopList[RIFLE_SLOT].GetDragDropItemsList().size() ? true : false;
			


			//m_pCurrentDragDropItem = pDDItemMP;			
			if (bRiffleAvailable)
				if (CanAttachAddOn((CUIDragDropItemMP*)UITopList[RIFLE_SLOT].GetDragDropItemsList().front(),m_pCurrentDragDropItem))
				{
						switch(ID){
						case CUIDragDropItemMP::ID_SILENCER:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_SILENCER_ADDON /*+ static_cast<int>(ID)*/);
							break;
						case CUIDragDropItemMP::ID_SCOPE:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_SCOPE_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_SCOPE_ADDON /*+ static_cast<int>(ID)*/);
							break;
						case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_GRENADE_LAUNCHER_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_GRENADE_LAUNCHER_ADDON /*+ static_cast<int>(ID)*/);
                    
					}
				}
		
			if (bPistolAvailable)
				if (CanAttachAddOn((CUIDragDropItemMP*)UITopList[PISTOL_SLOT].GetDragDropItemsList().front(),m_pCurrentDragDropItem))
					UIPropertiesBox.AddItem("Attach add-on to pistol", 
						NULL,
						pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON_PISTOL + static_cast<int>(ID) : ATTACH_SILENCER_ADDON_PISTOL + static_cast<int>(ID));
		}
		else
		{
			UIPropertiesBox.AddItem("Buy Item",  NULL, BUY_ITEM_ACTION);
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Cancel", NULL, CANCEL_BUYING_ACTION);
		// Так как оружие еще в сумке, то просматриваем его список аддонов и модифицируем меню
		if (m_pCurrentDragDropItem->bAddonsAvailable)
		{
			xr_string strMenuItem;
			CUIDragDropItemMP* pItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
					// If addon detached
				case 0:
					// Если денег на аддон хватает
					pItem = GetAddonByID(m_pCurrentDragDropItem, static_cast<CUIDragDropItemMP::AddonIDs>(i));
					if (HasEnoughMoney(pItem)/*pItem && pItem->GetCost() <= GetMoneyAmount()*/ && pItem->IsDragDropEnabled())
					{
						if (!this->UIBagWnd.IsItemInBag(pItem))
							break;
						strMenuItem = xr_string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
						
						switch(static_cast<CUIDragDropItemMP::AddonIDs>(i)){
						case CUIDragDropItemMP::ID_SILENCER:
							if (UITopList[RIFLE_SLOT].GetDragDropItemsList().front() == m_pCurrentDragDropItem)
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON);
							else
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON_PISTOL);
							break;
						case CUIDragDropItemMP::ID_SCOPE:
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SCOPE_ADDON);
							break;
						case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_GRENADE_LAUNCHER_ADDON);
							break;
						}
					}
					break;
				case 1:
					strMenuItem = xr_string("Detach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					switch(static_cast<CUIDragDropItemMP::AddonIDs>(i)){
					case CUIDragDropItemMP::ID_SILENCER:
						if (UITopList[RIFLE_SLOT].GetDragDropItemsList().front() == m_pCurrentDragDropItem)
							UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON + i);
						else
							UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON_PISTOL + i);
						break;
					case CUIDragDropItemMP::ID_SCOPE:
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SCOPE_ADDON);
						break;
					case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_GRENADE_LAUNCHER_ADDON);
						break;
					}

				default:
					continue;
				}
			}
		}
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}


bool CUIBuyWeaponWnd::ToSlot()
{
	if ( !UIBagWnd.IsItemInBag(m_pCurrentDragDropItem) )
		return false;


	// Специальное поведение для гранат
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	// Если вещь аддон
	CUIDragDropItemMP::AddonIDs ID;
	CUIDragDropItemMP * pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);
	if (pDDItemMP && !UIBagWnd.IsItemInBag(pDDItemMP))
        pDDItemMP->AttachDetachAddon(ID, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
	

	// Если вещь без номера слота, то ее поместить никуда нельзя (например патроны)
	int itemSlot = m_pCurrentDragDropItem->GetSlot();
	if (itemSlot < MP_SLOTS_NUM && itemSlot >= 0)
	{
		SlotToSection(itemSlot);

		m_pCurrentDragDropItem->MoveOnNextDrop();
//		m_pCurrentDragDropItem->GetParent()->SendMessage(
		UITopList[itemSlot].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

		BuyReaction();

		m_pMouseCapturer = NULL;
		return true;
	}
	return false;
}

void CUIBuyWeaponWnd::BuyReaction(){
	int iActiveIndex = UITabControl.GetActiveIndex();

	if (2 == iActiveIndex || 4 == iActiveIndex)
	{
		; // do nothing
	}
	else
		UITabControl.SetActiveState();
}

bool CUIBuyWeaponWnd::ToBag()
{
	// if in bag
	if (UIBagWnd.IsItemInBag(m_pCurrentDragDropItem))
		return false;

	// if in belt
	if (BeltToSection(m_pCurrentDragDropItem)) 
	{
		HighlightCurAmmo();	
		return true;
	}

	// if in slot
	if (SlotToSection(m_pCurrentDragDropItem->GetSlot())) 
	{
		HighlightCurAmmo();
		return true;
	}

	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// cheking
	if (!CanPutInBelt(m_pCurrentDragDropItem)							||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent()	||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)		||
		IsItemAnAddonSimple(m_pCurrentDragDropItem)						||
		!UIBagWnd.IsItemInBag(m_pCurrentDragDropItem)
		)
		return false;

	m_pCurrentDragDropItem->MoveOnNextDrop();
	m_pCurrentDragDropItem->AttachDetachAllAddons(false);
	UITopList[BELT_SLOT].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

	m_pMouseCapturer = NULL;
	return true;
}

bool CUIBuyWeaponWnd::SlotToSection(const u32 SlotNum)
{
	if (SlotNum >= MP_SLOTS_NUM) return false;
	// Выкидываем вещь из слота если есть.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		pDDItemMP->MoveOnNextDrop();
		pDDItemMP->AttachDetachAllAddons(false);
		UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);

		//CUIDragDropList* list = );
//		pDDItemMP->GetOwner()->SortList(MP_item_cmp);
	}

	return true;
}

bool CUIBuyWeaponWnd::CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum)
{
	return slotNum == pDDItemMP->GetSlot();
}

const u8 CUIBuyWeaponWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idx);

	return UIBagWnd.GetItemIndex(pDDItemMP, sectionNum);	
}

const u8 CUIBuyWeaponWnd::GetWeaponIndex(u32 slotNum)
{
	u8 tmp;
	return GetItemIndex(slotNum, 0, tmp);
}

const u8 CUIBuyWeaponWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId)
{
	itemId = GetItemIndex(BELT_SLOT, indexInBelt, sectionId);
	return itemId;
}

void CUIBuyWeaponWnd::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx)
{
	UIBagWnd.GetWeaponIndexByName(sectionName, grpNum, idx);
}

char*	CUIBuyWeaponWnd::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
	return UIBagWnd.GetWeaponNameByIndex(grpNum, idx);
}

CUIDragDropItemMP * CUIBuyWeaponWnd::GetWeapon(u32 slotNum, u32 idx)
{
	R_ASSERT(slotNum < MP_SLOTS_NUM);

	if (UITopList[slotNum].GetDragDropItemsList().empty()) 
		return NULL;

	R_ASSERT(idx < UITopList[slotNum].GetDragDropItemsList().size());

	DRAG_DROP_LIST_it it = UITopList[slotNum].GetDragDropItemsList().begin(); 
	std::advance(it, idx);
	return smart_cast<CUIDragDropItemMP*>(*it);
}

const u8 CUIBuyWeaponWnd::GetWeaponAddonInfoByIndex(u8 idx)
{
	return (idx & 0xe0) >> 5;
}

bool CUIBuyWeaponWnd::CanPutInBelt(CUIDragDropItemMP *pDDItemMP)
{
	return pDDItemMP->GetSlot() == GRENADE_SLOT || pDDItemMP->GetSlot() > 7;
}

bool CUIBuyWeaponWnd::BeltToSection(CUIDragDropItemMP *pDDItemMP)
{
	R_ASSERT(pDDItemMP);

	if (&UITopList[BELT_SLOT] != pDDItemMP->GetParent()) return false;
	// Берем вещь
	pDDItemMP->MoveOnNextDrop();
	// ...и посылаем ему сообщение переместиться в сумку
	UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);

	return true;
}

const u8 CUIBuyWeaponWnd::GetBeltSize()
{
	return static_cast<u8>(UITopList[BELT_SLOT].GetDragDropItemsList().size());
}

bool CUIBuyWeaponWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_RELEASED == keyboard_action) 
		return true;

	if (UIPropertiesBox.GetVisible())
		UIPropertiesBox.OnKeyboard(dik, keyboard_action);

	switch (UIBagWnd.GetMenuLevel())
	{
	case mlRoot:
		if (UITabControl.OnKeyboard(dik, keyboard_action))
			return true;
		break;
	default:
		if (UIBagWnd.OnKeyboard(dik, keyboard_action))
			return true;
	}

	if (DIK_ESCAPE == dik)
		UIBtnCancel.OnClick();
	else if (g_iOkAccelerator == dik || DIK_SPACE == dik)
		UIBtnOK.OnClick();
	else if (DIK_B == dik)
		UITabControl.SetActiveState();
	else if (DIK_C == dik)
		UIBtnClear.OnClick();	
	else if (DIK_Q == dik && UIBtn_PistolBullet.IsEnabled())
		OnBtnBulletBuy(PISTOL_SLOT);
	else if (DIK_W == dik && UIBtn_RifleBullet.IsEnabled())
		OnBtnBulletBuy(RIFLE_SLOT);
	else if (DIK_E == dik && UIBtn_RifleGranade.IsEnabled())
		OnBtnRifleGranade();
	else if (DIK_A == dik && UIBtn_PistolSilencer.IsEnabled())
		OnBtnSilencerBuy(PISTOL_SLOT);
	else if (DIK_F == dik && UIBtn_RifleSilencer.IsEnabled())
		OnBtnSilencerBuy(RIFLE_SLOT);
	else if (DIK_S == dik && UIBtn_RifleScope.IsEnabled())
		OnBtnRifleScope();
	else if (DIK_D == dik && UIBtn_RifleGranadelauncer.IsEnabled())
		OnBtnRifleGranadelauncher();


	return true;
}

bool CUIBuyWeaponWnd::CheckBuyAvailabilityInSlots()
{
	// массив приоритетов слотов при проверке
	const int arrSize = 5;
	int priorityArr[arrSize] =
	{
		RIFLE_SLOT,
		PISTOL_SLOT,
		OUTFIT_SLOT,
		GRENADE_SLOT,
		KNIFE_SLOT
	};

	bool status = true;

	// Пробегаемся по вещам в слотах, и смотрим есть ли
	// денежки на эту вещь. Если есть, то отнимаем цену вещи
	// от суммы денег, и продолжанм провнрку
	for (int j = 0; j < arrSize; ++j)
	{
		// Если вещь есть
		if (!UITopList[priorityArr[j]].GetDragDropItemsList().empty())
		{
			DRAG_DROP_LIST &pList = UITopList[priorityArr[j]].GetDragDropItemsList();

			for (DRAG_DROP_LIST_it it = pList.begin(); it != pList.end(); ++it)
			{
				CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
				R_ASSERT(pDDItemMP);

				// И если на нее хватает денег, то отнимаем ее цену от количества денег
				// Если эта вещь принесена игроком, то деньги не отнимаем
				if (!pDDItemMP->m_bHasRealRepresentation)
				{
					if (HasEnoughMoney(pDDItemMP)/*pDDItemMP->GetCost() <= GetMoneyAmount()*/)
					{
//						SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
						SetMoneyAmount(GetMoneyLeft(- pDDItemMP->GetCost()));
						pDDItemMP->SetColor(cAbleToBuy);
					}
					else
					{
						pDDItemMP->SetColor(cUnableToBuy);
						status = false;

						// Для армора закрашиваем дополнительно и иконку с изображением персонажа
						if (OUTFIT_SLOT == pDDItemMP->GetSlot())
							UIOutfitIcon.SetColor(cUnableToBuy);
						pDDItemMP->m_bAlreadyPaid = false;
					}
				}
				else
				{
					pDDItemMP->SetColor(cAbleToBuyOwned);

					// Для армора закрациваем дополнительно и иконку с изображением персонажа
					if (OUTFIT_SLOT == pDDItemMP->GetSlot())
						UIOutfitIcon.SetColor(cAbleToBuyOwned);
				}
			}
		}
	}

	// У пояса наименьший приоритет
	for (DRAG_DROP_LIST_it it = UITopList[BELT_SLOT].GetDragDropItemsList().begin();
		 it != UITopList[BELT_SLOT].GetDragDropItemsList().end();
		 ++it)
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
		R_ASSERT(pDDItemMP);

		if (!pDDItemMP->m_bHasRealRepresentation)
		{
			if (HasEnoughMoney(pDDItemMP)/*pDDItemMP->GetCost() <= GetMoneyAmount()*/)
			{
//				SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
				SetMoneyAmount(GetMoneyLeft(- pDDItemMP->GetCost()));
				pDDItemMP->SetColor(cAbleToBuy);
			}
			else
			{
				pDDItemMP->SetColor(cUnableToBuy);
				pDDItemMP->m_bAlreadyPaid = false;
				status = false;
			}
		}
		else
			pDDItemMP->SetColor(cAbleToBuyOwned);
	}

	return status;
}

void CUIBuyWeaponWnd::SetSkin(u8 SkinID)
{
	LPCSTR skins = pSettings->r_string(m_StrSectionName, "skins");

	R_ASSERT(_GetItemCount(skins) > SkinID);

	string64 item;
	_GetItem(skins, SkinID, item);

	m_current_skin = item;
	if (0!=xr_strcmp(m_current_skin.c_str(),item))
        UpdateOutfit();
}

void CUIBuyWeaponWnd::ClearSlots()
{
	IgnoreMoney(true);
	SlotToSection(KNIFE_SLOT);
	SlotToSection(PISTOL_SLOT);
	SlotToSection(RIFLE_SLOT);
	SlotToSection(GRENADE_SLOT);
	SlotToSection(APPARATUS_SLOT);
	SlotToSection(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		BeltToSection(static_cast<CUIDragDropItemMP*>(*it));

	IgnoreMoney(false);
}

int CUIBuyWeaponWnd::GetPriceOfOwnItems(){
	int price = 0;

	price += GetPriceOfItemInSlot(KNIFE_SLOT);
	price += GetPriceOfItemInSlot(PISTOL_SLOT);
	price += GetPriceOfItemInSlot(RIFLE_SLOT);
	price += GetPriceOfItemInSlot(GRENADE_SLOT);	
	price += GetPriceOfItemInSlot(APPARATUS_SLOT);
	price += GetPriceOfItemInSlot(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		price += GetItemPrice((CUIDragDropItemMP*)*it);

	return price;
}

int CUIBuyWeaponWnd::GetPriceOfItemInSlot(int slot){
	if (slot >= MP_SLOTS_NUM) 
		return 0;


	if (!UITopList[slot].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[slot].GetDragDropItemsList().begin());
		if (!pDDItemMP->m_bAlreadyPaid)
			return 0;
        return GetItemPrice(pDDItemMP);	
	}

	return 0;

}

void CUIBuyWeaponWnd::SectionToSlot(const char *sectionName, bool bRealRepresentationSet)
{
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(sectionName);	
	int iSlot = pDDItem->GetSlot();
	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[iSlot].GetDragDropItemsList().empty() || GRENADE_SLOT == iSlot)
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}
}

void CUIBuyWeaponWnd::SectionToSlot(const char* sectionName, u8 addon_info, bool bRealRepresentationSet){
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(sectionName, /* bCreateOnFail = */true);
	int iSlot = pDDItem->GetSlot();
	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[iSlot].GetDragDropItemsList().empty() || GRENADE_SLOT == iSlot)
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			if (addon_info != 0)
			{
				if ((addon_info & 1) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
				if ((addon_info & 2) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
				if ((addon_info & 4) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
			}
		}
}

void CUIBuyWeaponWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
	// "Выкусываем" флаги аддонов
	u8 uAddonFlags = GetWeaponAddonInfoByIndex(uIndexInSlot);
	uIndexInSlot &= 0x1f; // 0x1f = 00011111;

	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(grpNum, uIndexInSlot);

	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[pDDItem->GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == pDDItem->GetSlot() || NO_ACTIVE_SLOT == pDDItem->GetSlot())
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			// Проверяем индекс на наличие флагов аддонов, и если они есть, то 
			// аттачим аддоны к мувнутому оружию
			if (uAddonFlags != 0)
			{
				if ((uAddonFlags & 1) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
				if ((uAddonFlags & 2) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
				if ((uAddonFlags & 4) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
			}
		}
}

void CUIBuyWeaponWnd::ClearRealRepresentationFlags()
{
	UIBagWnd.ClearRealRepresentationFlags();
}

void CUIBuyWeaponWnd::SetMoneyAmount(int moneyAmount)
{	
	UIBagWnd.UpdateMoney(moneyAmount);
	{
		UIBagWnd.UpdateBuyPossibility();
	};
}

bool CUIBuyWeaponWnd::CanBuyAllItems()
{
	for (int i = 0; i < MP_SLOTS_NUM; ++i)
	{
		for (DRAG_DROP_LIST_it it = UITopList[i].GetDragDropItemsList().begin();
			 it != UITopList[i].GetDragDropItemsList().end(); ++it)
		{
			if (cUnableToBuy == (*it)->GetColor())
				return false;
		}
	}

	return true;
}

int CUIBuyWeaponWnd::GetMoneyAmount() const
{	
//	return m_iMoneyAmount;	
	return UIBagWnd.GetMoneyAmount();
}

CUIDragDropItemMP * CUIBuyWeaponWnd::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID)
{
	return UIBagWnd.GetAddonByID(pAddonOwner, ID);
}

CUIDragDropItemMP * CUIBuyWeaponWnd::IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID)
{
	R_ASSERT(pPossibleAddon);

	// проверяем не аддон ли pPossibleAddon вещи в слот(е/ах)
	if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());

		if (pDDItemMP && pDDItemMP->bAddonsAvailable)
		{
			for (u8 j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
			{
				// Если один из типов аддонов
				if (pPossibleAddon->GetSectionName() == pDDItemMP->m_AddonInfo[j].strAddonName)
				{
					ID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
					return pDDItemMP;
				}
			}
		}
	}

	if (!UITopList[PISTOL_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[PISTOL_SLOT].GetDragDropItemsList().front());

		if (pDDItemMP && pDDItemMP->bAddonsAvailable)
		{
			for (u8 j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
			{
				// Если один из типов аддонов
				if (pPossibleAddon->GetSectionName() == pDDItemMP->m_AddonInfo[j].strAddonName)
				{
					ID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
					return pDDItemMP;
				}
			}
		}
	}

	return NULL;
}

int CUIBuyWeaponWnd::GetItemPrice(CUIDragDropItemMP* pDDItemMP){
	if (NULL == pDDItemMP)
		return 0;

	return static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1));
}

bool CUIBuyWeaponWnd::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	return UIBagWnd.IsItemAnAddonSimple(pPossibleAddon);
}

void CUIBuyWeaponWnd::FillItemInfo(CUIDragDropItemMP *pDDItemMP)
{
	{
		UIItemInfo.m_b_force_drawing		= false;
		UIItemInfo.UIName->SetText			("");
		UIItemInfo.UIWeight->SetText		("");
		UIItemInfo.UIItemImage->TextureOff	();
		UIItemInfo.UIDesc->Clear			();
		if (pDDItemMP)
		{
			UIItemInfo.m_b_force_drawing		= true;
			CStringTable stbl;
			if (pSettings->line_exist(pDDItemMP->GetSectionName(), "inv_name"))
				UIItemInfo.UIName->SetText(*stbl.translate(pSettings->r_string(pDDItemMP->GetSectionName(), "inv_name")));

			UIItemInfo.UIItemImage->SetShader(pDDItemMP->GetUIStaticItem().GetShader());
			Frect o_r = pDDItemMP->GetUIStaticItem().GetOriginalRect();
			Frect v_r = pDDItemMP->GetUIStaticItem().GetRect();
			UIItemInfo.UIItemImage->GetUIStaticItem().SetOriginalRect(o_r.x1, o_r.y1, o_r.width(), o_r.height());
			UIItemInfo.UIItemImage->GetUIStaticItem().SetRect(v_r);
			UIItemInfo.UIItemImage->SetStretchTexture(true);
			UIItemInfo.UIItemImage->SetWidth	(_min(v_r.width(),UIItemInfo.UIItemImageSize.x));
			UIItemInfo.UIItemImage->SetHeight(_min(v_r.height(),UIItemInfo.UIItemImageSize.y));
			UIItemInfo.UIItemImage->TextureOn();

			
			if (pDDItemMP->m_iRank>=0 && pDDItemMP->m_iRank<=4)
			{
				string64 tex_name;
//				string64 foo;
				string64 team;
				if (UIBagWnd.IsBlueTeamItem(pDDItemMP->GetSectionName()))
                    strcpy(team, "blue");
				else 
					strcpy(team, "green");

				sprintf(tex_name, "ui_hud_status_%s_0%d", team, pDDItemMP->m_iRank+1);
				
				//strconcat(tex_name,"ui_hud_status_",itoa(pDDItemMP->m_iRank,foo,10));
				UIDescRankIcon.InitTexture(tex_name);
			}

			UIItemInfo.TryAddWpnInfo(pDDItemMP->GetSectionName());

			if (pSettings->line_exist(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD))
			{
				CUIStatic* pItem = xr_new<CUIStatic>();
				pItem->SetWidth(UIItemInfo.UIDesc->GetDesiredChildWidth());
				pItem->SetText(*CStringTable().translate(pSettings->r_string(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD)));
				pItem->AdjustHeightToText();
				UIItemInfo.UIDesc->AddWindow(pItem, true);
			}

			
		}
	}
}

bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetTop());
	R_ASSERT2(this_inventory, "wrong parent addressed");

	CUIDragDropItemMP* pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	pDDItemMP->AttachDetachAllAddons(false);

	// Need to delete copy before adding
	//if (pDDItemMP->HasAmountControl())
 //       pDDItemMP->OnReturn();
	this_inventory->UIBagWnd.DeleteCopy(pDDItemMP);

	pDDItemMP->GetParent()->DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScaleX(), pDDItemMP->GetOwner()->GetItemsScaleY());

	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	if (pDDItemMP->m_bAlreadyPaid)
	{
//		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() + GetItemPrice(pDDItemMP)); 		
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyLeft(GetItemPrice(pDDItemMP)));

		// Если у вещи есть аддоны, то прибавляем и также и их половинную стоимость
		pDDItemMP->m_bAlreadyPaid = false;
	}

	if (this_inventory->HasEnoughMoney(pDDItemMP)/*pDDItemMP->GetCost() > this_inventory->GetMoneyAmount()*/ && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// Если это армор, то убедимся, что он стал видимым
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		pDDItemMP->Show(true);

	return false;
}

void	CUIBuyWeaponWnd::ReloadItemsPrices	()
{
	UIBagWnd.ReloadItemsPrices();
}

void	CUIBuyWeaponWnd::IgnoreMoneyAndRank(bool ignore)
{
	m_bIgnoreMoneyAndRank = ignore;
	UIBagWnd.IgnoreRank(ignore);
	UIBagWnd.IgnoreMoney(ignore);
};

void	CUIBuyWeaponWnd::IgnoreMoney(bool ignore)
{
	if (m_bIgnoreMoneyAndRank)
	{		
		UIBagWnd.IgnoreMoney(true);
	}
	else
	{		
		UIBagWnd.IgnoreMoney(ignore);
	};
}

bool	CUIBuyWeaponWnd::HasEnoughMoney(CUIDragDropItemMP* pItem)
{
	return UIBagWnd.HasEnoughMoney(pItem);
};

int		CUIBuyWeaponWnd::GetMoneyLeft	(int ItemCost)
{
	return UIBagWnd.GetMoneyLeft(ItemCost);
};
//
//void init___()
//{
//	luabind::functor<float>		m_functorRPM;
//	luabind::functor<float>		m_functorAccuracy;
//	luabind::functor<float>		m_functorDamage;
//	luabind::functor<float>		m_functorHandling;
//
//	LPCSTR str					= "ui_wpn_params.GetRPM";
//	bool	functor_exists		= ai().script_engine().functor(str ,m_functor);
//	VERIFY(functor_exists);
//	//call
//	if(m_functor.is_valid()){
//		float res = m_functor(p1,p2,p3,p4);
//	}
//}