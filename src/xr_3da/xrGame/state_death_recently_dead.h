////////////////////////////////////////////////////////////////////////////
//	Module 		: state_death_recently_dead.h
//	Created 	: 15.01.2004
//  Modified 	: 15.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Death state (recently dead)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateDeathRecentlyDead : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

public:
						CStateDeathRecentlyDead	(LPCSTR state_name);
	virtual				~CStateDeathRecentlyDead();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_death_recently_dead_inline.h"