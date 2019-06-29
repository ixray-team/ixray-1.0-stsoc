#pragma once
#include "../../state_base.h"

class CAI_Biting;

//////////////////////////////////////////////////////////////////////////
// CStateBitingWander
// Бродить по точкам графа
//////////////////////////////////////////////////////////////////////////

class CStateBitingWander : public CStateBase<CAI_Biting> {
protected:
	typedef CStateBase<CAI_Biting> inherited;

public:
						CStateBitingWander	(LPCSTR state_name);
	virtual				~CStateBitingWander	();

	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

