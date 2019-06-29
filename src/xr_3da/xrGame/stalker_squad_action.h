////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_squad_action.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker squad action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

class CStalkerActionSquad : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionSquad			(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};
