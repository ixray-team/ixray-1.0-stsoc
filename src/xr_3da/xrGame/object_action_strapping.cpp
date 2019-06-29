////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_strapping.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object strapping command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_strapping.h"

CObjectActionStrapping::CObjectActionStrapping	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionStrapping::initialize		()
{
	inherited::initialize();
}

void CObjectActionStrapping::execute			()
{
	inherited::execute();
}

void CObjectActionStrapping::finalize		()
{
	inherited::finalize();
}
