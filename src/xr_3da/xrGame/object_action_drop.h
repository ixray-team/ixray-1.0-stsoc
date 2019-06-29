////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_drop.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object drop command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CInventoryItem;

class CObjectActionDrop : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name = "");
	virtual void		initialize			();
};