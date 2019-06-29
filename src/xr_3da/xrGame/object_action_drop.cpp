////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_drop.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object drop command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_drop.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionDrop::CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionDrop::initialize		()
{
	inherited::initialize	();
	if (!m_item || !m_item->H_Parent() || (m_object->ID() != m_item->H_Parent()->ID()))
		return;

	NET_Packet				P;
	m_object->u_EventGen	(P,GE_OWNERSHIP_REJECT,m_object->ID());
	P.w_u16					(u16(m_item->ID()));
	m_object->u_EventSend	(P);
}
