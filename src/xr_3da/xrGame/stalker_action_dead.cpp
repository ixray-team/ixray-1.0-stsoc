////////////////////////////////////////////////////////////////////////////
//	Module 		: action_dead.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action dead class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_action_dead.h"
#include "ai/stalker/ai_stalker.h"

CStalkerActionDead::CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDead::execute		()
{
	inherited::execute		();
	m_object->CMovementManager::enable_movement(false);
#ifndef OLD_DECISION_BLOCK
	m_object->set_dead		(true);
#endif
}
