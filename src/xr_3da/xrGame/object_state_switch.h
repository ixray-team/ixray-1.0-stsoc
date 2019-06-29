////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state_fire_primary.h
//	Created 	: 17.01.2004
//  Modified 	: 17.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state fire primary
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

class CObjectStateSwitch : public CObjectStateBase {
public:
						CObjectStateSwitch	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality = false) :
							CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	void		finalize			()
	{
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
		inherited::finalize();
	}
};