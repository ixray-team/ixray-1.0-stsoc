////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state.h
//	Created 	: 16.01.2004
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"
#include "weapon.h"

class CAI_Stalker;
class CInventoryItem;

class CObjectStateBase : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;
	CInventoryItem			*m_inventory_item;
	u32						m_weapon_state;
	bool					m_equality;
	bool					m_started;


private:
	u32						m_priority;

public:
						CObjectStateBase	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality = false);
	virtual				~CObjectStateBase	();
			void		Init				(CInventoryItem *inventory_item, const u32 weapon_state, bool equality);
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Stalker *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
	virtual	bool		completed			() const;
};

#include "state_free_no_alife_inline.h"