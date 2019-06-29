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

class CObjectStateFirePrimary : public CObjectStateBase {
public:
						CObjectStateFirePrimary	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality = false) :
							CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	void		initialize			()
	{
		inherited::initialize();
		m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	}

	virtual	void		execute			()
	{
		inherited::execute();
		m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	}

	virtual	bool		completed			() const
	{
		return				(true);
	}

	virtual	void		finalize			()
	{
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
		inherited::finalize();
	}
};