

#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingSleep
// Сон
//////////////////////////////////////////////////////////////////////////

class CStateBitingSleep : public CStateBase<CAI_Biting> {
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingSleep	(LPCSTR state_name);
	virtual				~CStateBitingSleep	();

	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

