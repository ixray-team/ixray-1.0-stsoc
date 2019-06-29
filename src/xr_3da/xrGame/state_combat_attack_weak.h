////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_attack_weak.h
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state attacking weak enemy
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateAttackWeak : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

public:
						CStateAttackWeak	(LPCSTR state_name);
	virtual				~CStateAttackWeak	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Stalker *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

#include "state_combat_attack_weak_inline.h"