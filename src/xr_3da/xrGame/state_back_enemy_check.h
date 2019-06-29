////////////////////////////////////////////////////////////////////////////
//	Module 		: state_back_enemy_check.h
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Back enemy check state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateBackEnemyCheck : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

public:
						CStateBackEnemyCheck(LPCSTR state_name);
	virtual				~CStateBackEnemyCheck();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Stalker *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

#include "state_anomaly_check_inline.h"