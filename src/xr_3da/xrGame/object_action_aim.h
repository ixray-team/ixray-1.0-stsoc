////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_aim.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object aim command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CInventoryItem;

class CObjectActionAim : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
};