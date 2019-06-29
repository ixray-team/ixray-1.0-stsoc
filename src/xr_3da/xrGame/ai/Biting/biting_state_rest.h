#pragma once

#include "../../state_manager_state.h"

class CAI_Biting;

class CStateBitingRest : public CStateManagerState<CAI_Biting> {
protected:
	typedef CStateManagerState<CAI_Biting> inherited;

	enum ERestStates {
		eRS_Sleep			= u32(0),
		eRS_WalkGraphPoint,
		eALifeStateDummy	= u32(-1),
	};
public:
						CStateBitingRest		(LPCSTR state_name);
	virtual				~CStateBitingRest		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Biting *object);
	
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

