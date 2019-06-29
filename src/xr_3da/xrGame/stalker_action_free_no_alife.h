////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_action_free_no_alife.h
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action free no ALife class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;

class CStalkerActionFreeNoALife : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker> inherited;

public:
						CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute						();
};

