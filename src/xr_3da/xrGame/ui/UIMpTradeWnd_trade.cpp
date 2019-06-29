#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"

#include "UICellItem.h"
#include "UIDragDropListEx.h"
#include "UICellCustomItems.h"
#include <dinput.h>

bool CUIMpTradeWnd::TryToSellItem(SBuyItemInfo* sell_itm, bool do_destroy)
{
	SellItemAddons						(sell_itm, at_scope);
	SellItemAddons						(sell_itm, at_silencer);
	SellItemAddons						(sell_itm, at_glauncher);

	u32		_item_cost					= m_item_mngr->GetItemCost(sell_itm->m_name_sect, GetRank() );
	
	SetMoneyAmount						(GetMoneyAmount() + _item_cost);

	CUICellItem* _itm					= NULL;
	CUIDragDropListEx* list_from		= sell_itm->m_cell_item->OwnerList();
	if(list_from)
		_itm = list_from->RemoveItem(sell_itm->m_cell_item, false );
	else
		_itm	= sell_itm->m_cell_item;

	SBuyItemInfo* iinfo					= FindItem(_itm); //just detached

	u32 cnt_in_shop						= GetItemCount(sell_itm->m_name_sect, SBuyItemInfo::e_shop);

	iinfo->SetState						(SBuyItemInfo::e_sold);
	if(cnt_in_shop!=0 )
	{
		if(do_destroy) 
			DestroyItem					(iinfo);
	}else
	{//return to shop

		if(m_store_hierarchy->CurrentLevel().HasItem(iinfo->m_name_sect) )
		{
			CUIDragDropListEx* _new_owner		= m_list[e_shop];
			_new_owner->SetItem					(iinfo->m_cell_item);
			int accel_idx						= m_store_hierarchy->CurrentLevel().GetItemIdx(iinfo->m_name_sect);
			VERIFY								(accel_idx!=-1);
			iinfo->m_cell_item->SetAccelerator	( (accel_idx>10) ? 0 : DIK_1+accel_idx );
			iinfo->m_cell_item->SetCustomDraw	(xr_new<CUICellItemAccelDraw>());

		}
	}
	if(_item_cost!=0)
	{
		string64							buff;
		sprintf								(buff,"+%d RU",_item_cost);
		SetInfoString						(buff);
	}

	UpdateCorrespondingItemsForList			(list_from);
	return true;
}

bool CUIMpTradeWnd::BuyItemAction(SBuyItemInfo* itm)
{
	CUIDragDropListEx*	_list	= NULL;
	u8 list_idx					= m_item_mngr->GetItemSlotIdx(itm->m_name_sect);
	VERIFY						(list_idx<e_total_lists && list_idx!=e_shop);
	if(list_idx==e_pistol || list_idx==e_rifle || list_idx==e_outfit)
	{
		_list						= m_list[list_idx];
		if( _list->ItemsCount() )
		{
			VERIFY					(_list->ItemsCount()==1);
			CUICellItem* ci			= _list->GetItemIdx(0);
			
			if(ci->EqualTo(itm->m_cell_item))
			{
				return false;
			}

			SBuyItemInfo* to_sell	= FindItem(ci); 
			SBuyItemInfo::EItmState	_stored_state = to_sell->GetState();

			TryToSellItem			(to_sell, false);

			bool b_res				= TryToBuyItem(itm, bf_normal, NULL);

			if(!b_res)
			{
				to_sell->SetState	(SBuyItemInfo::e_undefined);	//hack
				bool b_res2			= TryToBuyItem(to_sell, bf_ignore_restr, NULL);
				R_ASSERT			(b_res2);
				to_sell->SetState	(SBuyItemInfo::e_undefined);	//hack
				to_sell->SetState	(_stored_state);				//hack
			}else
				DestroyItem			(to_sell);

			return					b_res;
		}
	}
	
	return TryToBuyItem	(itm, bf_normal, NULL);
}

bool CUIMpTradeWnd::TryToBuyItem(SBuyItemInfo* buy_itm, u32 buy_flags, SBuyItemInfo* itm_parent)
{
	SBuyItemInfo* iinfo 			= buy_itm;
	const shared_str& buy_item_name = iinfo->m_name_sect;
	
	bool	b_can_buy		= CheckBuyPossibility(buy_item_name, buy_flags);
	if(!b_can_buy)
		return				false;

	u32 _item_cost				= m_item_mngr->GetItemCost(buy_item_name, GetRank() );

	if(! (buy_flags&bf_ignore_money) )
	{
		SetMoneyAmount				(GetMoneyAmount() - _item_cost);
	}

	if( (buy_flags&bf_own_item) == bf_own_item )
	{
		iinfo->SetState				(SBuyItemInfo::e_own);
	}else
		iinfo->SetState				(SBuyItemInfo::e_bought);


	CUICellItem* cell_itm				= NULL;
	if(iinfo->m_cell_item->OwnerList())// just from shop
		cell_itm					= iinfo->m_cell_item->OwnerList()->RemoveItem(iinfo->m_cell_item, false );
	else //new created
		cell_itm					= iinfo->m_cell_item;


	bool b_addon					= TryToAttachItemAsAddon(iinfo, itm_parent);
	if(!b_addon)
	{
		CUIDragDropListEx*_new_owner	= NULL;
		_new_owner						= GetMatchedListForItem(buy_item_name);
		_new_owner->SetItem				(cell_itm);
		cell_itm->SetCustomDraw			(NULL);
		cell_itm->SetAccelerator		(0);
		UpdateCorrespondingItemsForList	(_new_owner);

	}else{
		DestroyItem					(iinfo);
	}

	RenewShopItem					(buy_item_name, true);

	if( (buy_flags&bf_normal) && _item_cost!=0)
	{
		string64					buff;
		sprintf						(buff,"-%d RU",_item_cost);
		SetInfoString				(buff);
	}
	return						true;
}

bool CUIMpTradeWnd::CheckBuyPossibility(const shared_str& sect_name, u32 buy_flags)
{
	string256					info_buffer;
	bool b_can_buy				= true;

	u32 _item_cost				= m_item_mngr->GetItemCost(sect_name, GetRank() );

	if( !(buy_flags&bf_ignore_money) )
	{
		if( GetMoneyAmount() < _item_cost)
		{
			sprintf					(	info_buffer,"Cant buy item. Not enought money. has[%d] need[%d]", 
										GetMoneyAmount(), 
										_item_cost);
			b_can_buy				= false;
		};
	}

	if(b_can_buy && !(buy_flags&bf_ignore_restr) && !g_mp_restrictions.IsAvailable(sect_name))
	{
		sprintf					(	info_buffer,"Cant buy item. Rank restrictions. has[%s] need[%s] ", 
									g_mp_restrictions.GetRankName(GetRank()).c_str(), 
									g_mp_restrictions.GetRankName(get_rank(sect_name)).c_str());
		b_can_buy				= false;
	}

	if(b_can_buy && !(buy_flags&bf_ignore_restr))
	{
		const shared_str& group = g_mp_restrictions.GetItemGroup(sect_name);
		u32 cnt_restr			= g_mp_restrictions.GetGroupCount(group);
		
		u32 cnt_have			=  GetGroupCount					(group, SBuyItemInfo::e_bought);
			cnt_have			+= GetGroupCount					(group, SBuyItemInfo::e_own);

		if(cnt_have>=cnt_restr)
		{
		sprintf					(	info_buffer,"Cant buy item. Count restrictions. You already have [%d] item of this type", 
									cnt_have);
		b_can_buy				= false;
		}
	}

	if(!b_can_buy)
	{
		SetInfoString			(info_buffer);
	};
	
	return						b_can_buy;
}

void CUIMpTradeWnd::RenewShopItem(const shared_str& sect_name, bool b_just_bought)
{
	if(m_store_hierarchy->CurrentLevel().HasItem(sect_name) )
	{
		CUIDragDropListEx*	pList			= m_list[e_shop];
		SBuyItemInfo* pitem					= CreateItem(sect_name, SBuyItemInfo::e_shop, true);
		if(pitem->m_cell_item->OwnerList()!=pList)
		{
			int accel_idx						= m_store_hierarchy->CurrentLevel().GetItemIdx(sect_name);
			pitem->m_cell_item->SetAccelerator	( (accel_idx>9) ? 0 : DIK_1+accel_idx );

			pitem->m_cell_item->SetCustomDraw	(xr_new<CUICellItemAccelDraw>());
			pList->SetItem						(pitem->m_cell_item);
		}
	}
}

void CUIMpTradeWnd::ItemToBelt(const shared_str& sectionName)
{
	R_ASSERT2(m_item_mngr->GetItemIdx(sectionName)!=u32(-1), sectionName.c_str() );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);
}

void CUIMpTradeWnd::ItemToRuck(const shared_str& sectionName, u8 addons)
{
	R_ASSERT2(m_item_mngr->GetItemIdx(sectionName)!=u32(-1), sectionName.c_str() );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);

	SetItemAddonsState_ext				(pitem, addons);
}

void CUIMpTradeWnd::ItemToSlot(const shared_str& sectionName, u8 addons)
{
	R_ASSERT2(m_item_mngr->GetItemIdx(sectionName)!=u32(-1), sectionName.c_str() );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);
	SetItemAddonsState_ext				(pitem, addons);
}
