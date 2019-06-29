////////////////////////////////////////////////////////////////////////////
//	Module 		: state_death_already_dead.h
//	Created 	: 15.01.2004
//  Modified 	: 15.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Death state (already dead)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateDeathAlreadyDead : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

public:
						CStateDeathAlreadyDead	(LPCSTR state_name);
	virtual				~CStateDeathAlreadyDead	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_death_already_dead_inline.h"