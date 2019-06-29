////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_script.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_action_manager_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

void CMotivationActionManager<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMotivationActionManager,CScriptMotivationActionManagerWrapper,bases<CScriptMotivationManager,CScriptActionPlanner> >("motivation_action_manager")
			.def(								constructor<>())
			.def("setup",						&CScriptMotivationActionManager::setup,		&CScriptMotivationActionManagerWrapper::setup_static)
			.def("update",						&CScriptMotivationActionManager::update,	&CScriptMotivationActionManagerWrapper::update_static)
			.def("clear",						&CScriptMotivationActionManager::clear)
			.def("clear_motivations",			&CScriptMotivationActionManager::clear_motivations)
			.def("clear_actions",				&CScriptMotivationActionManager::clear_actions)
			.def("add_action",					&CScriptMotivationActionManager::add_action,adopt(_3))
//			.def("add_action",					(void (CScriptMotivationActionManager::*)(const CScriptActionPlanner::_edge_type &, CScriptActionPlannerAction::COperator *))(CScriptMotivationActionManager::add_operator),adopt(_3))
	];
}
