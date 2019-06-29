#pragma once

#include "../../state_manager_state.h"
class CAI_Biting;

class CStateBitingAttack : public CStateManagerState<CAI_Biting> {
protected:
	typedef CStateManagerState<CAI_Biting> inherited;

	enum EAttackStates {
		eAS_Run				= u32(0),
		eAS_Melee,
		eALifeStateDummy	= u32(-1),
	};


public:
						CStateBitingAttack		(LPCSTR state_name);
	virtual				~CStateBitingAttack		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Biting *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

