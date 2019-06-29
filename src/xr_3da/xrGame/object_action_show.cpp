////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_show.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object show command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_show.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionShow::CObjectActionShow	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionShow::initialize		()
{
	inherited::initialize			();
	VERIFY							(m_item);
	m_object->inventory().Slot		(m_item);
	m_object->inventory().Activate	(m_item->GetSlot());
}
