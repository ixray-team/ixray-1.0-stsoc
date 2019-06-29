#pragma once

#define 	BELT_SLOT			5
#define 	MP_SLOTS_NUM		8
#define		WEAPON_BOXES_SLOT	MP_SLOTS_NUM + 1

#define		MAX_ITEMS	70

const u32			cDetached					= 0xffffffff;
const u32			cAttached					= 0xff00ff00;
const u32			cUnableToBuy				= 0xffff8080;
const u32			cUnableByRank				= 0x88ff9999;
const u32			cAbleToBuy					= cDetached;
const u32			cAbleToBuyOwned				= 0xff9696ff;
const float			SECTION_ICON_SCALE			= 4.0f/5.0f;
const char * const	BUY_WND_XML_NAME			= "inventoryMP.xml";
const float			fRealItemSellMultiplier		= 0.5f;
const char * const	weaponFilterName			= "weapon_class";
const char * const	BUY_MP_ITEM_XML				= "buy_mp_item.xml";
const char * const	WEAPON_DESCRIPTION_FIELD	= "description";
