////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_threaten.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object threaten command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CObjectActionThreaten : public CObjectActionBase<CAI_Stalker> {
protected:
	typedef CObjectActionBase<CAI_Stalker> inherited;

protected:
	bool				*m_value;

public:
						CObjectActionThreaten	(CAI_Stalker *item, CAI_Stalker *owner, bool *value, LPCSTR action_name = "");
	virtual void		execute					();
};