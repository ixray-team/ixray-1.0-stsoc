#pragma once

#include "../../state_manager_state.h"

class CAI_Biting;

class CStateBitingPanic : public CStateManagerState<CAI_Biting> {
protected:
	typedef CStateManagerState<CAI_Biting> inherited;

	enum EPanicStates {
		ePS_RunAway,
		ePS_FaceBackScared,
		eALifeStateDummy	= u32(-1),
	};
public:
						CStateBitingPanic		(LPCSTR state_name);
	virtual				~CStateBitingPanic		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Biting *object);
	
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

