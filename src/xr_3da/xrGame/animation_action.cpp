////////////////////////////////////////////////////////////////////////////
//	Module 		: animation_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Animation action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "animation_action.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_movement_manager.h"

void CAnimationAction::execute	()
{
	object().movement().set_mental_state	(m_mental_state);
}