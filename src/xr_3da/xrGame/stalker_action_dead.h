////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_action_dead.h
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action dead class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;

class CStalkerActionDead : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker> inherited;

public:
						CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute				();
};

