////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state_reload.h
//	Created 	: 17.01.2004
//  Modified 	: 17.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state reload
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

class CObjectStateEmpty : public CObjectStateBase {
public:
							CObjectStateEmpty	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality = false) :
								CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	bool		completed			() const
	{
		CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
		if (!weapon)
			return		(inherited::completed());

		return					(
			((weapon->STATE != m_weapon_state) && !m_equality)
		);
	}
};