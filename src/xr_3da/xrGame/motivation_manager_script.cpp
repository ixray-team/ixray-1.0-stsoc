////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager_script.cpp
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_manager_wrapper.h"
#include "script_space.h"
#include "script_game_object.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

void CMotivationManager<CScriptGameObject>::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMotivationManager,CScriptMotivationManagerWrapper>("motivation_manager")
			.def(								constructor<>())
			.def("add_motivation",				&CScriptMotivationManager::add_motivation,adopt(_3))
			.def("remove_motivation",			&CScriptMotivationManager::remove_motivation)
			.def("add_connection",				&CScriptMotivationManager::add_connection)
			.def("remove_connection",			&CScriptMotivationManager::remove_connection)
			.def("motivation",					&CScriptMotivationManager::motivation)
			.def("selected",					&CScriptMotivationManager::selected)
			.def("selected_id",					&CScriptMotivationManager::selected_id)
			.def("setup",						&CScriptMotivationManager::setup,	&CScriptMotivationManagerWrapper::setup_static)
			.def("update",						&CScriptMotivationManager::update,	&CScriptMotivationManagerWrapper::update_static)
			.def("clear",						&CScriptMotivationManager::clear)
	];
}
