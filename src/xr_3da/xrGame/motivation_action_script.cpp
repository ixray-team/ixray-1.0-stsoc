////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_script.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_action_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"
#include "script_world_state.h"

using namespace luabind;

void CMotivationAction<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMotivationAction,CScriptMotivationActionWrapper,CScriptMotivation>("motivation_action")
			.def(								constructor<const CScriptWorldState &>())
			.def("goal",						&CScriptMotivationAction::goal)
			.def("setup",						&CScriptMotivationAction::setup,	&CScriptMotivationActionWrapper::setup_static)
			.def("evaluate",					&CScriptMotivationAction::evaluate,	&CScriptMotivationActionWrapper::evaluate_static)
	];
}
