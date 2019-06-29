////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_show.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object show command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CInventoryItem;

class CObjectActionSwitch : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionSwitch	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
	virtual void		finalize			();
};