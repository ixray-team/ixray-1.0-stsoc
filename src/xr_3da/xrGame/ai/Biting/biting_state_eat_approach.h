

#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingEatApproach
// Подойти к трупу
//////////////////////////////////////////////////////////////////////////

class CStateBitingEatApproach : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

	float				cur_dist;

public:
						CStateBitingEatApproach	(LPCSTR state_name);
	virtual				~CStateBitingEatApproach();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

	virtual	bool		completed				() const;
};

