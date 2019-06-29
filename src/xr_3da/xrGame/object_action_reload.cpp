////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_reload.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object reload command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_reload.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionReload::CObjectActionReload	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_type			(type)
{
}

void CObjectActionReload::initialize		()
{
	inherited::initialize		();
	m_object->inventory().Action(kWPN_RELOAD,	CMD_START);
}

void CObjectActionReload::execute			()
{
	inherited::execute();
	m_object->inventory().Action(kWPN_RELOAD,	CMD_START);
}

