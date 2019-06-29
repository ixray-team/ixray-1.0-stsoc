////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_unstrapping.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object unstrapping command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_unstrapping.h"

CObjectActionUnstrapping::CObjectActionUnstrapping	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionUnstrapping::initialize		()
{
	inherited::initialize();
}

void CObjectActionUnstrapping::execute			()
{
	inherited::execute();
}

void CObjectActionUnstrapping::finalize		()
{
	inherited::finalize();
}
