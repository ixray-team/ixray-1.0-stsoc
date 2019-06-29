#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingAttackRun
// Бежать к врагу
//////////////////////////////////////////////////////////////////////////

class CStateBitingAttackRun : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingAttackRun	(LPCSTR state_name);
	virtual				~CStateBitingAttackRun	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

};
