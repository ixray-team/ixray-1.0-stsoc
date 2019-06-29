////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_fire.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object fire command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_fire.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionFire::CObjectActionFire	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_type			(type)
{
}

void CObjectActionFire::initialize		()
{
	inherited::inherited::initialize	();
	m_object->inventory().Action(kWPN_FIRE,	CMD_START);
}

void CObjectActionFire::execute			()
{
	inherited::execute			();
	m_object->inventory().Action(kWPN_FIRE,	CMD_START);
}

void CObjectActionFire::finalize		()
{
	inherited::finalize();
	m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
}