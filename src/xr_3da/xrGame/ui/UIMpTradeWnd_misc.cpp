#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"

#include "UICellItem.h"
#include "UITabControl.h"
#include "UIDragDropListEx.h"
#include "UIItemInfo.h"
#include "UI3tButton.h"

#include "../inventory_item.h"
#include "../PhysicsShellHolder.h"
#include "../object_broker.h"
#include <dinput.h>

bool CUIMpTradeWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
#ifdef DEBUG
	//for debug only
	if(keyboard_action==WINDOW_KEY_PRESSED && dik==DIK_NUMPAD7)
	{
		if(GetRank()>0)
			SetRank( clampr(u32(GetRank()-1),u32(0),u32(4) ) );
	}
	if(keyboard_action==WINDOW_KEY_PRESSED && dik==DIK_NUMPAD8)
	{
		SetRank( clampr(u32(GetRank()+1),u32(0),u32(4) ) );
	}
#endif

	if(!m_store_hierarchy->CurrentIsRoot())
	{
		if (m_shop_wnd->OnKeyboard(dik, keyboard_action) )
			return true;

		m_root_tab_control->SetAcceleratorsMode		(false);
	}

	bool res =  inherited::OnKeyboard(dik, keyboard_action);

	m_root_tab_control->SetAcceleratorsMode		(true);

	return			res;
}

void CUIMpTradeWnd::Update()
{
	inherited::Update			();
	UpdateMomeyIndicator		();
}

void CUIMpTradeWnd::UpdateMomeyIndicator()
{
	string128						buff;
	sprintf							(buff, "%d", m_money);
	m_static_player_money->SetText	(buff);
	//update preset money
	
	for(u32 i=_preset_idx_last; i<=_preset_idx_3; ++i)
	{
		CUIStatic* st				= m_static_preset_money[i];
		u32	_cost					= GetPresetCost((ETradePreset)i);
		sprintf						(buff, "%d", _cost);
		st->SetText					(buff);
		bool b_has_enought_money	= _cost<=GetMoneyAmount();
		u32 clr						= (b_has_enought_money)?color_rgba(231,153,22,255):color_rgba(255,0,0,255);
		st->SetTextColor			(clr);
		const preset_items&		v	=  GetPreset((ETradePreset)i);
		m_btns_preset[i]->Enable	(b_has_enought_money && v.size()!=0);
	}
}

void CUIMpTradeWnd::SetInfoString(LPCSTR str)
{
	m_static_information->SetText			(str);
	m_static_information->ResetClrAnimation	();
	Msg("Buy menu message:%s", str);
}

void CUIMpTradeWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent		(pWnd, msg, pData);
}

CUICellItem* CUIMpTradeWnd::CurrentItem()
{
	return		m_pCurrentCellItem;
}

CInventoryItem* CUIMpTradeWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(CInventoryItem*)m_pCurrentCellItem->m_pData : NULL;
}
LPCSTR _team_names[]=
{
"green",
"blue"
};
void CUIMpTradeWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm)		return;
	m_pCurrentCellItem					= itm;
	m_item_info->InitItem				(CurrentIItem());
	if (m_pCurrentCellItem)
	{
		const shared_str& current_sect_name = CurrentIItem()->object().cNameSect();
		string256						str;
		sprintf							(str, "%d", GetItemPrice(CurrentIItem()));
		m_item_info->UICost->SetText	(str);

		m_item_info->UIName->SetText	(CurrentIItem()->NameShort());

		string64						tex_name;
		string64						team;

		if (m_store_hierarchy->FindItem(current_sect_name) )
		{// our team
			strcpy						(team, _team_names[m_store_hierarchy->TeamIdx()]);
		}else 
		{
			strcpy						(team, _team_names[m_store_hierarchy->TeamIdx()%1]);
		}
		sprintf							(tex_name, "ui_hud_status_%s_0%d", team, 1+get_rank(current_sect_name.c_str()) );
				
		m_static_item_rank->InitTexture		(tex_name);
		m_static_item_rank->TextureOn		();
	}
	else
	{
		m_static_item_rank->TextureOff		();
	}
}


int	CUIMpTradeWnd::GetItemPrice(CInventoryItem* itm)
{
	return m_item_mngr->GetItemCost(itm->object().cNameSect(), g_mp_restrictions.GetRank());
}

void CUIMpTradeWnd::BindDragDropListEvents(CUIDragDropListEx* lst, bool bDrag)
{
	lst->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemDrop);
	lst->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemDbClick);
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemSelected);
	lst->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemRButtonClick);
	if(bDrag)
		lst->m_f_item_start_drag	= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemStartDrag);

}

bool CUIMpTradeWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	_owner_list		= itm->OwnerList();
	dd_list_type		_owner_type		= GetListType	(_owner_list);

	CUIDragDropListEx*	_new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	
	if(!_new_owner || _new_owner==_owner_list)		
		return			true;

	dd_list_type		_new_owner_type	= GetListType	(_new_owner);

	SBuyItemInfo*		iinfo 		= FindItem(itm);

	if(_owner_type==dd_shop)
	{
		BuyItemAction	(iinfo);
//.		VERIFY			(res);
		return			true;
	}

	if(_new_owner_type==dd_shop)
	{
		bool res		= TryToSellItem			(iinfo, true);
		VERIFY			(res);
		return			true;
	}
	
	if(_new_owner_type==dd_own_bag)
	{
		CUICellItem* citm				= _owner_list->RemoveItem(itm, false);
		_new_owner->SetItem				(citm);
		UpdateCorrespondingItemsForList	(_owner_list);
	}else
	{
		VERIFY(_new_owner_type==dd_own_slot);
		if( _new_owner==GetMatchedListForItem(iinfo->m_name_sect) )
		{
			CUICellItem* citm				= _owner_list->RemoveItem(itm, false);
			_new_owner->SetItem				(citm);
			UpdateCorrespondingItemsForList	(_new_owner);
		}
	}	
	return							true;
}

bool CUIMpTradeWnd::OnItemStartDrag(CUICellItem* itm)
{
	return							false;
}

bool CUIMpTradeWnd::OnItemDbClick(CUICellItem* itm)
{
	CUIDragDropListEx*	_owner_list		= itm->OwnerList();
	dd_list_type		_owner_type		= GetListType	(_owner_list);

	SBuyItemInfo*		iinfo 		= FindItem(itm);

	switch(_owner_type)
	{
		case dd_shop:
			{
				BuyItemAction		(iinfo);
			}break;

		case dd_own_bag:
		case dd_own_slot:
			TryToSellItem			(iinfo, true);
			break;
		default:					NODEFAULT;
	};

	return							true;
}

bool CUIMpTradeWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem					(itm);
	return							false;
}

bool CUIMpTradeWnd::OnItemRButtonClick(CUICellItem* itm)
{
	return							false;
}


CUIMpTradeWnd::dd_list_type CUIMpTradeWnd::GetListType(CUIDragDropListEx* l)
{
	if(l==m_list[e_shop])		return dd_shop;
	if(l==m_list[e_player_bag])	return dd_own_bag;
	
	return						dd_own_slot;
}

CUIDragDropListEx*	CUIMpTradeWnd::GetMatchedListForItem(const shared_str& sect_name)
{
	CUIDragDropListEx*	res		= NULL;
	u8 list_idx					= m_item_mngr->GetItemSlotIdx(sect_name);
	VERIFY						(list_idx<e_total_lists && list_idx!=e_shop);
	res							= m_list[list_idx];

	//special case
	if(list_idx==e_pistol_ammo || list_idx==e_rifle_ammo)
	{
		CUICellItem* ci = (m_list[list_idx-1]->ItemsCount())?m_list[list_idx-1]->GetItemIdx(0):NULL;
		if(!ci)
			return				m_list[e_player_bag];

		CInventoryItem* ii = (CInventoryItem*)ci->m_pData;

		if(!ii->IsNecessaryItem(sect_name))
			return				m_list[e_player_bag];
	}
	return						res;
}


const u32 CUIMpTradeWnd::GetRank() const
{
	return g_mp_restrictions.GetRank();
}


/// iBuyWnd
const u8 CUIMpTradeWnd::GetWeaponIndex(u32 slotNum)
{
	return 0;
}

shared_str _fake;
const shared_str& CUIMpTradeWnd::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
//	return _fake;
	if (idx >= m_item_mngr->GetItemsCount()) return _fake;
	return m_item_mngr->GetItemName(u32(idx));
}


const u8 CUIMpTradeWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count)
{
	return 0;
}

void CUIMpTradeWnd::GetWeaponIndexByName(const shared_str& sectionName, u8 &grpNum, u8 &idx)
{
	
	u32 idx__	= m_item_mngr->GetItemIdx(sectionName);
	grpNum		= 0;
	idx			= (u8)idx__;
}

const u8 CUIMpTradeWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	return 0;
}

const u8 CUIMpTradeWnd::GetBeltSize()
{
	return 0;
}

void CUIMpTradeWnd::ClearSlots()
{}

void CUIMpTradeWnd::ClearRealRepresentationFlags()
{}

void CUIMpTradeWnd::IgnoreMoneyAndRank(bool ignore)
{}

void CUIMpTradeWnd::IgnoreMoney(bool ignore)
{}

u32 CUIMpTradeWnd::GetMoneyAmount() const
{
	return			m_money;
}

u32 CUIMpTradeWnd::GetPresetCost(ETradePreset idx)
{
	const preset_items&		v			=  GetPreset(idx);
	preset_items::const_iterator it		= v.begin();
	preset_items::const_iterator it_e	= v.end();

	u32 result							= 0;
	for(;it!=it_e;++it)
	{
		const _preset_item& _one		= *it;

		u32 _item_cost					= m_item_mngr->GetItemCost(_one.sect_name, GetRank() );

		if(_one.addon_names[0].c_str())
			_item_cost					+= m_item_mngr->GetItemCost(_one.addon_names[0], GetRank() );
		
		if(_one.addon_names[1].c_str())
			_item_cost					+= m_item_mngr->GetItemCost(_one.addon_names[1], GetRank() );

		if(_one.addon_names[2].c_str())
			_item_cost					+= m_item_mngr->GetItemCost(_one.addon_names[2], GetRank() );

		_item_cost						*= _one.count;

		result							+= _item_cost;
	}
	return result;
}

void CUIMpTradeWnd::SetSkin(u8 SkinID)
{}

void CUIMpTradeWnd::SetRank(u32 rank)
{
	g_mp_restrictions.SetRank(rank);

	string64			tex_name;
	string64			team;

	strcpy				(team, _team_names[m_store_hierarchy->TeamIdx()]);
	sprintf				(tex_name, "ui_hud_status_%s_0%d", team, 1+rank );
			
	m_static_player_rank->InitTexture(tex_name);
	m_static_player_rank->TextureOn();
}


void CUIMpTradeWnd::SetMoneyAmount(u32 money)
{
	VERIFY			(money>=0);
	m_money			= money;
}

void CUIMpTradeWnd::ResetItems()
{
	ResetToOrigin						();
	m_store_hierarchy->Reset			();
	UpdateShop							();
	SetCurrentItem						(NULL);
}

bool CUIMpTradeWnd::CanBuyAllItems()
{
	return true;
}

void CUIMpTradeWnd::AddonToSlot(int add_on, int slot, bool bRealRepresentationSet)
{
// own

}

void CUIMpTradeWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
// own
}

bool CUIMpTradeWnd::CheckBuyAvailabilityInSlots()
{
	return true;
}
