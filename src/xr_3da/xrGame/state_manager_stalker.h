////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CAI_Stalker;

class CStateManagerStalker : public CStateManagerState<CAI_Stalker> {
protected:
	typedef CStateManagerState<CAI_Stalker> inherited;

	enum EStalkerStates {
		eStalkerStateDeath = u32(0),
		eStalkerStateNoALife,
		eStalkerStateALife,
		eStalkerStateCombat,
		eStalkerStateDummy = u32(-1),
	};
public:
						CStateManagerStalker	(LPCSTR state_name);
	virtual				~CStateManagerStalker	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_manager_stalker_inline.h"