////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_command.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object action command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CInventoryItem;

class CObjectActionCommand : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_command;

public:
						CObjectActionCommand(CInventoryItem *item, CAI_Stalker *owner, u32 command, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
};