////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_switch.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object switch command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_switch.h"

CObjectActionSwitch::CObjectActionSwitch	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_type			(type)
{
}

void CObjectActionSwitch::initialize		()
{
	inherited::initialize	();
}

void CObjectActionSwitch::execute			()
{
	inherited::execute		();
}

void CObjectActionSwitch::finalize		()
{
	inherited::finalize		();
}
