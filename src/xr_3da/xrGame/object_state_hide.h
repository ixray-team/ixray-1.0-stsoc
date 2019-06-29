////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state_hide.h
//	Created 	: 17.01.2004
//  Modified 	: 17.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state hide
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

class CObjectStateHide : public CObjectStateBase {
public:
						CObjectStateHide	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality = false) :
							CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	void		initialize			()
	{
		inherited::initialize();
		VERIFY			(m_inventory_item);
		m_object->inventory().Activate(NO_ACTIVE_SLOT);
	}
};