////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_threaten.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object threaten command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_threaten.h"
#include "inventory.h"
#include "ai/stalker/ai_stalker.h"

CObjectActionThreaten::CObjectActionThreaten	(CAI_Stalker *item, CAI_Stalker *owner, bool *value, LPCSTR action_name) :
	inherited				(item,owner,action_name),
	m_value					(value)
{
}

void CObjectActionThreaten::execute			()
{
	inherited::execute		();
#ifndef OLD_OBJECT_HANDLER
	if (completed()) {
		*m_value			= false;
		m_object->inventory().Action(kWPN_FIRE,	CMD_STOP);
	}
//	else
//		m_object->inventory().Action(kWPN_FIRE,	CMD_START);
#endif
}
