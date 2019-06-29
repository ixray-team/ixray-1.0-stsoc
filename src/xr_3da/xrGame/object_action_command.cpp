////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_command.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object action command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_command.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectActionCommand::CObjectActionCommand(CInventoryItem *item, CAI_Stalker *owner, u32 command, LPCSTR action_name) :
	inherited			(item,owner,action_name),
	m_command			(command)
{
}

void CObjectActionCommand::initialize	()
{
	inherited::initialize();
	m_object->inventory().Action(m_command,CMD_START);
}

void CObjectActionCommand::execute		()
{
	inherited::execute();
}
