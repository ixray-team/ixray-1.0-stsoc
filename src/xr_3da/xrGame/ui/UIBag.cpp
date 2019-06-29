// File:		UIBag.cpp
// Description:	Bag for BuyWeaponWnd
// Created:		10.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include <dinput.h>
#include "UIBuyWeaponStd.h"
#include "UIBag.h"
#include "UIInventoryUtilities.h"
#include "../HUDManager.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "Restrictions.h"
#include "../level.h"
#include "../game_base_space.h"
//#ifdef DEBUG
//#include "../../xr_ioconsole.h"
//#include "../../xr_ioc_cmd.h"
//#endif //#ifdef DEBUG

CRestrictions g_mp_restrictions;
using namespace InventoryUtilities;


CUIBag::CUIBag(CHECK_PROC proc){
	g_mp_restrictions.InitGroups();

	for (int i=0; i<4; i++)
        subSection_group3[i] = -1;

	m_mlCurrLevel	= mlRoot;
	m_pCurrentDDItem = NULL;
	m_iMoneyAmount = 10000;
//	m_iCurrentRank = 0;

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		AttachChild(&m_groups[i]);
		m_groups[i].SetCheckProc(proc);
		m_groups[i].SetItemsScaleXY(SECTION_ICON_SCALE, SECTION_ICON_SCALE);
	}

	AttachChild(&m_btnBack);

	m_boxesDefs[0].xmlTag		= "btn_bag_shotgun";
	m_boxesDefs[0].filterString	= "shotgun";
	m_boxesDefs[0].gridHeight		= 2;
	m_boxesDefs[0].gridWidth		= 6;

	m_boxesDefs[1].xmlTag		= "btn_bag_assault";
	m_boxesDefs[1].filterString	= "assault_rifle";
	m_boxesDefs[1].gridHeight		= 2;
	m_boxesDefs[1].gridWidth		= 6;

	m_boxesDefs[2].xmlTag		= "btn_bag_sniper";
	m_boxesDefs[2].filterString	= "sniper_rifle";
	m_boxesDefs[2].gridHeight		= 2;
	m_boxesDefs[2].gridWidth		= 6;

	m_boxesDefs[3].xmlTag		= "btn_bag_heavy";
	m_boxesDefs[3].filterString	= "heavy_weapon";
	m_boxesDefs[3].gridHeight		= 2;
	m_boxesDefs[3].gridWidth		= 6;

	m_bIgnoreRank					= false;
	m_bIgnoreMoney					= false;
}

CUIBag::~CUIBag(){
	xr_vector<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
		xr_delete(*it);
}

void CUIBag::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx){
	grpNum	= (u8)(-1);
	idx		= (u8)(-1);

	for (u8 i = 0; i < m_wpnSectStorage.size(); ++i)
	{
		for (u8 j = 0; j < m_wpnSectStorage[i].size(); ++j)
		{
			if (sectionName == m_wpnSectStorage[i][j])
			{
				grpNum	= i;
				idx		= j;
				return;
			}
		}
	}
}

char* CUIBag::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
	if (grpNum >= m_wpnSectStorage.size()) return NULL;
	if (idx >= m_wpnSectStorage[grpNum].size()) return NULL;
	return (char*) m_wpnSectStorage[grpNum][idx].c_str();
}

u8 CUIBag::GetItemIndex(CUIDragDropItemMP* pDDItem, u8 &sectionNum){
	sectionNum = 0;
	u8 returnID = static_cast<u8>(-1);

	if (!pDDItem)
		return returnID;
    
	returnID = static_cast<u8>(pDDItem->GetPosInSectionsGroup());
	sectionNum = static_cast<u8>(pDDItem->GetSectionGroupID());

	// Проверяем на наличие приаттаченых аддонов к оружию
	if (pDDItem->bAddonsAvailable)
	{
		u8	flags = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (1 == pDDItem->m_AddonInfo[i].iAttachStatus)
				flags |= 1;
			flags = flags << 1;
		}
		flags = flags << 4;
		// В результате старшие 3 бита являются флагами признаков аддонов:
		// FF - Scope, FE - Silencer, FD - Grenade Launcher
		returnID |= flags;
	}
	return returnID;
}

MENU_LEVELS CUIBag::GetMenuLevel(){
	return m_mlCurrLevel;
}

bool CUIBag::SetMenuLevel(MENU_LEVELS level){
	// check range
	if (level < mlRoot || level > mlWpnSubType) 
		return false;

	m_btnBack.Enable(mlRoot != level);

	// check we really change state
	if (m_mlCurrLevel == level)
		return false;

	m_mlCurrLevel = level;	

	GetMessageTarget()->SendMessage(this, XR_MENU_LEVEL_CHANGED, NULL);

	return true;
}

int  CUIBag::GetMoneyAmount() const {
	return m_iMoneyAmount;
}

void CUIBag::Init(CUIXml& xml, const char *path, LPCSTR strSectionName, LPCSTR strPricesSection){
	m_StrSectionName = strSectionName;
	m_StrPricesSection = strPricesSection;
	
	CUIXmlInit::InitStatic(xml, path, 0, this);

	for (int i = 0; i < NUMBER_OF_GROUPS; i++){
		CUIXmlInit::InitDragDropList(xml, "dragdrop_list_bag", 0, &m_groups[i]);
		m_groups[i].SetMessageTarget(GetParent());
	}

	CUIXmlInit::Init3tButton(xml, "bag_back_btn", 0, &m_btnBack);

	InitBoxes(xml);
	InitWpnSectStorage();
	FillUpGroups();
	HideAll();
	SetMenuLevel(mlRoot);
}

int CUIBag::GetItemRank(const char* item){
	char rank[16];
	char foo[5];

	// from 4 downto 1
	for (int i = 4; i>=0; i--)
	{
		strconcat(rank,"rank_",itoa(i,foo,10));
		if (IsInRank(item,rank))
			return i;
	}

	return 255;	
}

bool CUIBag::IsInRank(const char* item, const char* rank){
	if(!pSettings->section_exist(rank))
		return false;

	xr_string itemsList; 
	string256 single_item;

	itemsList = pSettings->r_string(rank, "available_items");
	int itemsCount	= _GetItemCount(itemsList.c_str());

	for (int i = 0; i < itemsCount; i++)
	{
		_GetItem(itemsList.c_str(), i, single_item);		
		if (0 == xr_strcmp(item, single_item))
			return true;
	}    
	return false;
}

void CUIBag::Init(float x, float y, float width, float height){
	CUIStatic::Init(x, y, width, height);
}

void CUIBag::UpdateBuyPossibility(){
	CUIDragDropItemMP* pDDItem;
	CUIDragDropList* currentDDList;
//	int money = 
		GetMoneyAmount();
	bool flag;

	for (int i = GROUP_2; i<=GROUP_34; i++)
	{
		if (GROUP_BOXES == i)
			continue;

		currentDDList = &m_groups[i];
		if (!currentDDList)
			return;


		DRAG_DROP_LIST ddList = currentDDList->GetDragDropItemsList();
		DRAG_DROP_LIST_it it;

		for (it = ddList.begin(); it != ddList.end(); ++it)
		{
			pDDItem = (CUIDragDropItemMP*)(*it);
//			flag = !(pDDItem->GetCost() > money);
			flag = (pDDItem->m_bAlreadyPaid || HasEnoughMoney(pDDItem));
			EnableDDItem(pDDItem, flag);
			EnableDDItemByRank(pDDItem);
		}
	}

	
}

CUIDragDropItemMP* CUIBag::GetItemBySectoin(const char* sectionName, bool bCreateOnFail){

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		DRAG_DROP_LIST list = m_groups[i].GetDragDropItemsList();
		DRAG_DROP_LIST_it it;
		for (it = list.begin(); it != list.end(); ++it)
		{
			CUIDragDropItemMP* pDDItem = smart_cast<CUIDragDropItemMP*>(*it);
			if (0 == xr_strcmp(pDDItem->GetSectionName(), sectionName))
				return pDDItem;
		}
	}	

	if (bCreateOnFail && pSettings->line_exist(m_StrPricesSection, sectionName))
	{
		for (u32 i = 0; i < m_allItems.size(); i++)
			if (0 == xr_strcmp(m_allItems[i]->GetSectionName(), sectionName))
				return NULL;

		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		FillUpItem(pNewDDItem, sectionName);
		pNewDDItem->SetSectionGroupID(0);
		pNewDDItem->SetPosInSectionsGroup(0);
		PutItemToGroup(pNewDDItem, GROUP_DEFAULT);
		m_allItems.push_back(pNewDDItem);
		return pNewDDItem;
	}

	return NULL;
}

CUIDragDropItemMP* CUIBag::GetItemBySectoin(const u8 grpNum, u8 uIndexInSlot){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		DRAG_DROP_LIST list = m_groups[i].GetDragDropItemsList();
		DRAG_DROP_LIST_it it;
		for (it = list.begin(); it != list.end(); ++it)
		{
			CUIDragDropItemMP* pDDItem = smart_cast<CUIDragDropItemMP*>(*it);
			if (grpNum == pDDItem->GetSectionGroupID() && uIndexInSlot == pDDItem->GetPosInSectionsGroup())
				return pDDItem;
		}
	}

	return NULL;
}

#define HIGHTLIGHT_ITEM(x)		x->SetLightAnim("ui_slow_blinking", true, true, false, true)
#define UNHIGHTLIGHT_ITEM(x)	x->SetLightAnim(NULL, true, true, false, true); \
								x->SetColor(0xffffffff)

void CUIBag::HighLightAmmo(const char* slot1, const char* slot2){
		CUIDragDropList* currentDDList = &m_groups[GROUP_4];
		CUIDragDropItemMP* pDDItem = NULL;

		// unhightlight all
		DRAG_DROP_LIST ddList = currentDDList->GetDragDropItemsList();
		DRAG_DROP_LIST_it it;
		for (it = ddList.begin(); it != ddList.end(); ++it)
		{
			pDDItem = (CUIDragDropItemMP*)(*it);
			UNHIGHTLIGHT_ITEM(pDDItem);
		}

        HighLightAmmo(slot1);
        HighLightAmmo(slot2);
}

void CUIBag::HighLightAmmo(const char* weapon){
	if (!(weapon && weapon[0]))
		return;

	shared_str itemsList; 
	string256 single_item;

	itemsList = pSettings->r_string(weapon, "ammo_class");
	int itemsCount	= _GetItemCount(*itemsList);

	CUIDragDropItemMP* pDDItem = NULL;

//	xr_vector<shared_str> items;
	for (int i = 0; i < itemsCount; i++)
	{
		_GetItem(itemsList.c_str(), i, single_item);
		pDDItem = this->GetItemBySectoin(single_item);
		if (pDDItem)
		{
			HIGHTLIGHT_ITEM(pDDItem);			
		}
	}
}

void CUIBag::ClearRealRepresentationFlags(){
	xr_vector<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
	{
		(*it)->m_bHasRealRepresentation = false;
		EnableDDItem(*it);
	}		
}

void CUIBag::EnableDDItem(CUIDragDropItemMP* pDDItem, bool bEnable){
	bool owned = pDDItem->m_bHasRealRepresentation;
	u32 color = bEnable ? (owned ? cAbleToBuyOwned : cAbleToBuy) : cUnableToBuy;
	pDDItem->SetColor(color);
	pDDItem->EnableDragDrop(bEnable);
}

void CUIBag::EnableDDItemByRank(CUIDragDropItemMP* pDDItem){
	if (m_bIgnoreRank) return;
	if (pDDItem->m_iRank > g_mp_restrictions.GetRank())
	{
		pDDItem->SetColor(cUnableByRank);
		pDDItem->EnableDragDrop(false);
	}    
}

bool CUIBag::IsItemInBag(CUIDragDropItemMP* pDDItem){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if (m_groups[i].IsChild(pDDItem))
			return true;

	return false;
}

void CUIBag::Draw(){
	CUIStatic::Draw();
}

CUIDragDropItemMP * CUIBag::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID){
	R_ASSERT(pAddonOwner);
	// Пробегаемся по списку вещей и ищем там нужный аддон
	xr_vector<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it!=m_allItems.end(); ++it)
		for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
		{
			CUIDragDropItemMP::AddonIDs AID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
			if (pAddonOwner->m_AddonInfo[j].strAddonName == (*it)->GetSectionName() 
				&& AID == ID)
			{
				return *it;
			}
		}

	return NULL;
}

bool CUIBag::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	R_ASSERT(pPossibleAddon);
	if (!pPossibleAddon) return false;

	xr_string str = pPossibleAddon->GetSectionName();
	return str.find("addon") != xr_string::npos;
}



void CUIBag::OnItemDrop(CUIDragDropItemMP* pItem){
	CUIDragDropList* owner = smart_cast<CUIDragDropList*>(pItem->GetOwner());
	owner->SendMessage(pItem, DRAG_DROP_ITEM_DROP, NULL);	
}

void CUIBag::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIDragDropItemMP* pDDItem = smart_cast<CUIDragDropItemMP*>(pWnd);
	switch (msg)
	{
		// we using our super-puper tab buttons enstead of DragDropItems
		// those buttons uses TAB_CHANGED message for Click event
	case TAB_CHANGED:
		if		(pWnd == m_boxesDefs[0].pButton)
			OnBtnShotgunsClicked();
		else if (pWnd == m_boxesDefs[1].pButton)
			OnBtnMachinegunsClicked();
		else if (pWnd == m_boxesDefs[2].pButton)
			OnBtnSniperClicked();
		else if (pWnd == m_boxesDefs[3].pButton)
			OnBtnHeavyClicked();
		break;
	case DRAG_DROP_ITEM_DROP:
		OnItemDrop(pDDItem);
		return;

	case BUTTON_CLICKED:
		if (&m_btnBack == pWnd)
			OnBackClick();
		break;
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBag::OnBtnShotgunsClicked(){
	m_boxesDefs[0].pButton->OnClick();
	m_boxesDefs[0].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_31);
}
void CUIBag::OnBtnMachinegunsClicked(){
	m_boxesDefs[1].pButton->OnClick();
	m_boxesDefs[1].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_32);
}
void CUIBag::OnBtnSniperClicked(){
	m_boxesDefs[2].pButton->OnClick();
	m_boxesDefs[2].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_33);
}
void CUIBag::OnBtnHeavyClicked(){
	m_boxesDefs[3].pButton->OnClick();
	m_boxesDefs[3].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_34);
}

void CUIBag::OnBackClick(){
	int iGroup;

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBag on level <mlRoot> can't handle OnBackClick");
		break;
	case mlBoxes:
		ShowSectionEx(-1);
		break;
	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
			ShowSectionEx(GROUP_BOXES);
		else
			ShowSectionEx(-1);
		break;
	default:
		NODEFAULT;
	}
}

void CUIBag::UpdateMoney(int iMoney){
	m_iMoneyAmount = iMoney;
}

void CUIBag::HideAll(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		m_groups[i].Show(false);
}

// Init Boxes SLOT
void CUIBag::InitBoxes(CUIXml& xml){

	for (u32 i = 0; i < 4; ++i)
	{
		CUITabButtonMP* pNewBtn = xr_new<CUITabButtonMP>();
		pNewBtn->SetAutoDelete(true);
		pNewBtn->SetOrientation(O_HORIZONTAL);
		pNewBtn->SetMessageTarget(this);
		CUIXmlInit::Init3tButton(xml, *m_boxesDefs[i].xmlTag, 0, pNewBtn);
		m_boxesDefs[i].pButton = pNewBtn;
		m_groups[GROUP_BOXES].AttachChild(pNewBtn);
	}
}

bool CUIBag::OnKeyboard(int dik, EUIMessages keyboard_action){
	int iGroup;

	if (DIK_ESCAPE == dik)
	{
		m_btnBack.OnClick();
		return true;
	}

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBag on level <mlRoot> can't handle keyboard");
		break;
	case mlBoxes:

		if (DIK_ESCAPE == dik)
		{
			ShowSectionEx(-1);			return true;
		}

		switch (dik)
		{
		case DIK_1:
			OnBtnShotgunsClicked();					return true;
		case DIK_2:
			OnBtnMachinegunsClicked();				return true;
		case DIK_3:
			OnBtnSniperClicked();					return true;
		case DIK_4:
			OnBtnHeavyClicked();					return true;
		}
		break;

	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (DIK_ESCAPE == dik)
		{
			if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
				ShowSectionEx(GROUP_BOXES);
			else
				ShowSectionEx(-1);

			return true;
		}

		if (dik <= DIK_0 && dik >= DIK_1)
		{
			CUIDragDropItemMP* pDDItemMP = GetItemByKey(dik,GetCurrentGroupIndex());
			if (pDDItemMP)
				GetTop()->SendMessage(pDDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}
		break;
	default:
		NODEFAULT;
	}

	return false;
}

CUIDragDropItemMP* CUIBag::GetItemByKey(int dik, int section){
	R_ASSERT2((GROUP_2 <= section) && (section <= GROUP_34), "CUIBag::GetItemByKey() - invalid section number");
	CUIDragDropList* pDDList = &m_groups[section];
	unsigned int index = static_cast<u32>(dik - 2);
	if (pDDList->GetDragDropItemsList().size() >= index)
	{
		DRAG_DROP_LIST_it it = pDDList->GetDragDropItemsList().begin();
		
		for (; it != pDDList->GetDragDropItemsList().end(); ++it)
		{
			CUIDragDropItemMP *pDDItemMP = static_cast<CUIDragDropItemMP*>(*it);
			
			if (pDDItemMP->GetPosInSubSection() == index)
				return pDDItemMP;
		}
	}
	return NULL;
}

CUIDragDropList* CUIBag::GetCurrentGroup(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return &m_groups[i];
	return NULL;
}

int CUIBag::GetCurrentGroupIndex(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return i;

	return GROUP_DEFAULT;
}

// for external using
void CUIBag::ShowSection(int iSection){
	R_ASSERT2(  (GROUP_2 <= iSection) && (iSection <= GROUP_6 + 1), "CUIBag::ShowSection() - invalid section number");

	HideAll();

	if (GROUP_6 + 1 != iSection)
	{
        m_groups[iSection].Show(true);

		if ( GROUP_BOXES == iSection )
			SetMenuLevel(mlBoxes);
		else
			SetMenuLevel(mlWpnSubType);
	}
	else
		SetMenuLevel(mlRoot);
}

// for internal using
void CUIBag::ShowSectionEx(int iSection){
	HideAll();

	if (-1 == iSection)
	{
		SetMenuLevel(mlRoot);	return;
	}

	R_ASSERT2((GROUP_2 <= iSection) && (iSection <= GROUP_34), "CUIBag::ShowSectionEx() - invalid section number");

    m_groups[iSection].Show(true);

	if ( GROUP_BOXES == iSection )
		SetMenuLevel(mlBoxes);
	else
		SetMenuLevel(mlWpnSubType);
}

bool CUIBag::IsBlueTeamItem(LPCSTR item){
	if (GameID() == GAME_DEATHMATCH)
		return true;					//in deathmath always blue

	bool blue = !strstr(m_StrSectionName.c_str(),"team1");	//is our team blue

	//
	string64			wpnSection;
	string1024			wpnNames, wpnSingleName;
//	sprintf(wpnSection, "slot%i", i);
	for (int i = 1; i < 20; ++i)
	{
		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_StrSectionName, wpnSection)) 
			continue;

		
		std::strcpy(wpnNames, pSettings->r_string(m_StrSectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(wpnNames, j, wpnSingleName);
			if (0 == xr_strcmp(item,wpnSingleName))
				return blue;
		}
	}	
    
	return !blue;
}

// fill out
//				m_wpnSectStorage
//				m_ConformityTable

void CUIBag::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string64			wpnSection;
	shared_str			iconName;
	string1024			wpnNames, wpnSingleName;
	// Номер секции с арморами
	const int			armorSectionIndex = 5;

	// Поле strSectionName должно содержать имя секции
	R_ASSERT(m_StrSectionName != "");
	R_ASSERT2(pSettings->section_exist(m_StrSectionName), "Section doesn't exist");

	for (int i = 1; i < 20; ++i)
	{
		// Очищаем буфер
		wpnOneType.clear();

		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_StrSectionName, wpnSection)) 
		{
			m_wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		// Читаем данные этого поля
		std::strcpy(wpnNames, pSettings->r_string(m_StrSectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// теперь для каждое имя оружия, разделенные запятыми, заносим в массив
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(wpnNames, j, wpnSingleName);
			wpnOneType.push_back(wpnSingleName);

			// Для арморов дополнительно инициализируем таблицу соответсвия армора и 
			// и иконки персонажа в этом арморе
			if (armorSectionIndex == i)
			{
				iconName	= pSettings->r_string(m_StrSectionName, wpnSingleName);
				m_ConformityTable.push_back(std::make_pair<shared_str, shared_str>(wpnSingleName, iconName));
			}
		}

		if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
	}

	wpnOneType.clear();

	CInifile::Sect &sect = pSettings->r_section(m_StrPricesSection.c_str());
	for (CInifile::SectIt it = sect.begin(); it != sect.end(); it++)
	{
		u8 group_id, index;
		GetWeaponIndexByName((*it).first.c_str(), group_id, index);

		if ((u8)(-1) == group_id || (u8)(-1) == index) // item not found
		{
			wpnOneType.push_back((*it).first.c_str());			//
		}	
	}
	if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
}

void CUIBag::FillUpGroups()
{	
	for (WPN_LISTS::size_type i = 0; i < m_wpnSectStorage.size(); ++i)
		FillUpGroup(static_cast<u32>(i));
}

void CUIBag::FillUpGroup(const u32 group)
{
	for (WPN_SECT_NAMES::size_type j = 0; j < m_wpnSectStorage[group].size(); ++j)
	{
		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		FillUpItem(pNewDDItem, m_wpnSectStorage[group][j].c_str());
		pNewDDItem->SetSectionGroupID(group);
		pNewDDItem->SetPosInSectionsGroup(static_cast<u32>(j));
		PutItemToGroup(pNewDDItem, group);
		m_allItems.push_back(pNewDDItem);		
	}
}

void CUIBag::FillUpItem(CUIDragDropItemMP* pDDItem, const char* name){
		pDDItem->SetAutoDelete(false);

		pDDItem->CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		pDDItem->SetShader(GetEquipmentIconsShader());
		pDDItem->SetStretchTexture(true);
		pDDItem->SetColor(0xffffffff);

		//properties used by inventory menu
		int iGridWidth	= pSettings->r_u32(name, "inv_grid_width");
		int iGridHeight	= pSettings->r_u32(name, "inv_grid_height");
		int iXPos		= pSettings->r_u32(name, "inv_grid_x");
		int iYPos		= pSettings->r_u32(name, "inv_grid_y");

		u32 slot;
		if(pSettings->line_exist(name, "slot"))
			slot = pSettings->r_u32(name, "slot");
		else
			slot = NO_ACTIVE_SLOT;

		if (slot != NO_ACTIVE_SLOT && slot >= MP_SLOTS_NUM )
			slot = NO_ACTIVE_SLOT;
		
		pDDItem->SetSlot(slot);
		

		pDDItem->SetFont(UI()->Font()->pFontLetterica16Russian);

		// Читаем стоимость оружия
		string256 buff;
		if (pSettings->line_exist(m_StrSectionName, strconcat(buff, name, "_cost")))
			pDDItem->SetCost(pSettings->r_u32(m_StrSectionName, buff));
		else if (pSettings->line_exist(m_StrPricesSection, name))
			pDDItem->SetCost(pSettings->r_u32(m_StrPricesSection, name));
		else
		{
			string256	buf;
			sprintf(buf, "Cannot find price for an item %s in sections: %s, %s",
					name, *m_StrSectionName, *m_StrPricesSection);
			R_ASSERT2(false, buf);
		}

		// Для арморов читаем дополнительно координаты на текстуре с иконками персонажей для арморов
		if (OUTFIT_SLOT == slot)
		{
			// Для каждого армора, который может быть у нас в мультиплеере, читаем инфу
			// для иконки сталкера в полный рост в этом арморе и с нужным цветом
			for (CONFORMITY_TABLE_it it = m_ConformityTable.begin(); it != m_ConformityTable.end(); ++it)
			{
				// Информация о таком арморе есть
				if (0 == xr_strcmp(it->first, name))
				{
					//shared_str modelName = it->second;
					pDDItem->m_additionalInfo = *it->second;

//					int m_iSkinX = 0, m_iSkinY = 0;
//					sscanf(pSettings->r_string("multiplayer_skins", *modelName), "%i,%i", &m_iSkinX, &m_iSkinY);
//					pDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinX));
//					pDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinY));
				}
		
			}
		}

		InitAddonsInfo(*pDDItem, name);

		pDDItem->SetGridHeight(iGridHeight);
		pDDItem->SetGridWidth(iGridWidth);

		pDDItem->GetUIStaticItem().SetOriginalRect(
			float(iXPos * INV_GRID_WIDTH),
			float(iYPos * INV_GRID_HEIGHT),
			float(iGridWidth * INV_GRID_WIDTH),
			float(iGridHeight * INV_GRID_HEIGHT));


		pDDItem->SetSectionName(name);
		pDDItem->m_iRank = GetItemRank(name);
		pDDItem->SetMessageTarget(GetParent());
		pDDItem->SetCustomDraw(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
}

CUIDragDropItemMP* CUIBag::CreateCopy(CUIDragDropItemMP *pDDItem){
#ifdef DEBUG
	R_ASSERT2(pDDItem, "CUIBag::CreateCopy - argument is NULL");
#endif
	unsigned int group = pDDItem->GetSectionGroupID();
	unsigned int index = pDDItem->GetPosInSectionsGroup();

	CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
	pNewDDItem->SetAutoDelete(true);

	//FillUpItem(pNewDDItem, group, index);
	FillUpItem(pNewDDItem, m_wpnSectStorage[group][index].c_str());
	pNewDDItem->SetSectionGroupID(group);
	pNewDDItem->SetPosInSectionsGroup(index);
	PutItemToGroup(pNewDDItem, group);

	m_vCopyList.push_back(pNewDDItem);

	return pNewDDItem;
}

void CUIBag::DeleteCopy(CUIDragDropItemMP* pDDItem){
	shared_str section = pDDItem->GetSectionName();

	DRAG_DROP_LIST ddList = pDDItem->GetOwner()->GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = ddList.begin(); it != ddList.end(); ++it)
	{
        CUIDragDropItemMP* pItem = smart_cast<CUIDragDropItemMP*>(*it);
		shared_str it_section = pItem->GetSectionName();
		if (0 == xr_strcmp(section, it_section))
		{
			pItem->GetParent()->DetachChild(pItem);
			m_vCopyList.remove(pItem);
//			Msg("mp_buymenu: CUIBag::DeleteCopy(%s) - SUCCESS", pDDItem->GetSectionName());

#ifdef DEBUG
			DRAG_DROP_LIST ddList_2 = pDDItem->GetOwner()->GetDragDropItemsList();
			DRAG_DROP_LIST_it it_2;

		   	for (it_2 = ddList_2.begin(); it_2 != ddList_2.end(); ++it_2)
			{
		        CUIDragDropItemMP* pItem = smart_cast<CUIDragDropItemMP*>(*it_2);
				if (0 == xr_strcmp(section, pItem->GetSectionName()))
					R_ASSERT2(false, "CUIBag::DeleteCopy - two items with one section");
			}
#endif
			return;
		}
	}
//	Msg("mp_buymenu: CUIBag::DeleteCopy(%s) - can't find item",pDDItem->GetSectionName());
 
}

void CUIBag::PutItemToGroup(CUIDragDropItemMP* pDDItem, int iGroup){
	int iActiveSection = -1;
	shared_str weapon_class;


	switch (iGroup)
	{
	case 1:
		iActiveSection = GROUP_2;	break;
	case 2:
		weapon_class = pSettings->r_string(pDDItem->GetSectionName(), weaponFilterName);

		if		(  0 == xr_strcmp(m_boxesDefs[0].filterString, weapon_class) )
			iActiveSection = GROUP_31;

		else if (  0 == xr_strcmp(m_boxesDefs[1].filterString, weapon_class) )
			iActiveSection = GROUP_32;

		else if (  0 == xr_strcmp(m_boxesDefs[2].filterString, weapon_class) )
			iActiveSection = GROUP_33;

		else if (  0 == xr_strcmp(m_boxesDefs[3].filterString, weapon_class) )
			iActiveSection = GROUP_34;
		break;
	case 3:
		iActiveSection = GROUP_4;	break;
	case 4:
		iActiveSection = GROUP_5;	break;
	case 5:
		iActiveSection = GROUP_6;	break;
	default:
		iActiveSection = GROUP_DEFAULT;
	}

	R_ASSERT2( (-1 != iActiveSection), "CUIBag::PutItemToGroup - invalid section");

	m_groups[iActiveSection].AttachChild(pDDItem);
	pDDItem->SetOwner(&m_groups[iActiveSection]);

	if ( 2 == iGroup)
	{
		++subSection_group3[iActiveSection - GROUP_31];
		pDDItem->SetPosInSubSection(subSection_group3[iActiveSection - GROUP_31]);
	}
	else
		pDDItem->SetPosInSubSection(pDDItem->GetPosInSectionsGroup());

}

void CUIBag::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const xr_string &sectioName)
{
	// Для каждого аддона проверяем его запись в ltx файле, и заполняем структуру

	for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
	{
		xr_string status, strX, strY, name;

		switch (static_cast<CUIDragDropItemMP::AddonIDs>(i))
		{
		case CUIDragDropItemMP::ID_SILENCER:
			status	= "silencer_status";
			name	= "silencer_name";
			strX	= "silencer_x";
			strY	= "silencer_y";
			break;
		case CUIDragDropItemMP::ID_SCOPE:
			status	= "scope_status";
			name	= "scope_name";
			strX	= "scope_x";
			strY	= "scope_y";
			break;
		case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
			status	= "grenade_launcher_status";
			name	= "grenade_launcher_name";
			strX	= "grenade_launcher_x";
			strY	= "grenade_launcher_y";
			break;
		default:
			R_ASSERT(!"Unknown type of addon");
		}

		if (pSettings->line_exist(sectioName.c_str(), status.c_str())){
			if (pSettings->r_u32(sectioName.c_str(), status.c_str()) == 2)
			{
				DDItemMP.bAddonsAvailable = true;
				DDItemMP.m_AddonInfo[i].iAttachStatus = 0;
				DDItemMP.m_AddonInfo[i].strAddonName = pSettings->r_string(sectioName.c_str(), name.c_str());
				DDItemMP.m_AddonInfo[i].x = pSettings->r_u32(sectioName.c_str(), strX.c_str());
				DDItemMP.m_AddonInfo[i].y = pSettings->r_u32(sectioName.c_str(), strY.c_str());
			}
			else if (pSettings->line_exist(sectioName.c_str(), "grenade_launcher_status")){
				if (pSettings->r_s32(sectioName.c_str(), "grenade_launcher_status") == 1)
                    DDItemMP.m_AddonInfo[i].iAttachStatus = 2;//permanent granadelauncher
			}
		}
	}
}

void	CUIBag::ReloadItemsPrices	()
{
	string256 ItemCostStr = "";
	string256 RankStr = "";

	xr_vector<CUIDragDropItemMP*>::iterator it;
	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
	{
		R_ASSERT(pSettings->line_exist(m_StrPricesSection, (*it)->strName));
		(*it)->SetCost(pSettings->r_u32(m_StrPricesSection, (*it)->strName.c_str()));
		//-------------------------------------------------------------------------------
		strconcat(ItemCostStr, (*it)->strName.c_str(), "_cost");
		if (pSettings->line_exist(m_StrSectionName, ItemCostStr))
			(*it)->SetCost(pSettings->r_u32(m_StrSectionName, ItemCostStr));
		//-------------------------------------------------------------------------------
		for (int i=1; i<=g_mp_restrictions.GetRank(); i++)
		{
			string16 tmp;
			strconcat(RankStr, "rank_", itoa(i, tmp, 10));
			if (!pSettings->line_exist(RankStr, ItemCostStr)) continue;
			(*it)->SetCost(pSettings->r_u32(RankStr, ItemCostStr));
		}
	};
};

void	CUIBag::SetRank(int rank) 
{
	g_mp_restrictions.SetRank(rank);	
	ReloadItemsPrices();
};

bool	CUIBag::HasEnoughMoney(CUIDragDropItemMP* pItem)
{
	if (m_bIgnoreMoney) return true;
	R_ASSERT(pItem);
	if (!pItem) return false;
	return pItem->GetCost() <= GetMoneyAmount();
};

int		CUIBag::GetMoneyLeft	(int ItemCost)
{
	if (m_bIgnoreMoney) return GetMoneyAmount();
	return GetMoneyAmount() + ItemCost;
};