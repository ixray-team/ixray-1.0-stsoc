////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_aim.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object aim command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_aim.h"
#include "ai/stalker/ai_stalker.h"

CObjectActionAim::CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited				(item,owner,action_name),
	m_type					(type)
{
}

void CObjectActionAim::initialize		()
{
	inherited::inherited::initialize	();
#ifndef OLD_OBJECT_HANDLER
	m_object->set_aimed		(m_type ? 0 : 1,false);
#endif
}

void CObjectActionAim::execute			()
{
	inherited::execute		();
#ifndef OLD_OBJECT_HANDLER
	if (completed())
		m_object->set_aimed	(m_type,true);
#endif
}
