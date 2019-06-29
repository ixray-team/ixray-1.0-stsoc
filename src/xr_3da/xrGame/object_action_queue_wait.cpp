////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_queue_wait.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object queu wait command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_queue_wait.h"

CObjectActionQueueWait::CObjectActionQueueWait	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited				(item,owner,action_name),
	m_type					(type)
{
}

void CObjectActionQueueWait::initialize		()
{
	inherited::inherited::initialize	();
#ifndef OLD_OBJECT_HANDLER
	m_object->set_aimed		(m_type ? 0 : 1,false);
#endif
}

void CObjectActionQueueWait::execute			()
{
	inherited::execute		();
}

void CObjectActionQueueWait::finalize		()
{
	inherited::finalize		();
}
