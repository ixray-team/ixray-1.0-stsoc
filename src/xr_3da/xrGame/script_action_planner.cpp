////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner.cpp
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner.h"

void CScriptActionPlanner::reinit	(CLuaGameObject *object, bool clear_all)
{
	inherited::reinit	(object,clear_all);
}

void CScriptActionPlanner::update	(u32 time_delta)
{
	inherited::update	(time_delta);
}
