#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingEatEat
// Есть труп
//////////////////////////////////////////////////////////////////////////

class CStateBitingEatEat : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingEatEat	(LPCSTR state_name);
	virtual				~CStateBitingEatEat	();

	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();

	virtual	bool		completed			() const;
};

