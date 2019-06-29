////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_combat.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager combat
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CAI_Stalker;

class CStateManagerCombat : public CStateManagerState<CAI_Stalker> {
protected:
	typedef CStateManagerState<CAI_Stalker> inherited;

	enum ECombatStates {
		eCombatStateAttackWeak	= u32(0),
		eCombatStateCover,
		eCombatStateDummy		= u32(-1),
	};
public:
						CStateManagerCombat		(LPCSTR state_name);
	virtual				~CStateManagerCombat	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_manager_combat_inline.h"