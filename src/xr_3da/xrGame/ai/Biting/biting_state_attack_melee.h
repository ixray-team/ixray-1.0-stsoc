#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingAttackMelee
// Атаковать врага с близкого расстояния
//////////////////////////////////////////////////////////////////////////

class CStateBitingAttackMelee : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;


public:
						CStateBitingAttackMelee	(LPCSTR state_name);
	virtual				~CStateBitingAttackMelee();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

};
