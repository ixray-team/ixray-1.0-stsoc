////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_hide.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object hide command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_hide.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionHide::CObjectActionHide	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionHide::initialize		()
{
	inherited::initialize			();
	VERIFY							(m_item);
	m_object->inventory().Activate	(NO_ACTIVE_SLOT);
}