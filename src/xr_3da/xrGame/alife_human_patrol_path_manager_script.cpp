////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_patrol_path_manager_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human patrol path manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_patrol_path_manager.h"
#include "script_space.h"

using namespace luabind;

void CALifeHumanPatrolPathManager::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CALifeHumanPatrolPathManager>("CALifeHumanPatrolPathManager")
			.def("path",					(void (CALifeHumanPatrolPathManager::*)(LPCSTR))(&CALifeHumanPatrolPathManager::path))
			.def("start_type",				(void (CALifeHumanPatrolPathManager::*)(const EPatrolStartType	&))(&CALifeHumanPatrolPathManager::start_type))
			.def("start_type",				(const EPatrolStartType	&(CALifeHumanPatrolPathManager::*)() const)(&CALifeHumanPatrolPathManager::start_type))
			.def("route_type",				(void (CALifeHumanPatrolPathManager::*)(const EPatrolRouteType	&))(&CALifeHumanPatrolPathManager::route_type))
			.def("route_type",				(const EPatrolRouteType	&(CALifeHumanPatrolPathManager::*)() const)(&CALifeHumanPatrolPathManager::route_type))
			.def("actual",					&CALifeHumanPatrolPathManager::actual)
			.def("completed",				&CALifeHumanPatrolPathManager::completed)
			.def("start_vertex_index",		&CALifeHumanPatrolPathManager::start_vertex_index)
			.def("use_randomness",			(void (CALifeHumanPatrolPathManager::*)(const bool &))(&CALifeHumanPatrolPathManager::use_randomness))
			.def("use_randomness",			(bool (CALifeHumanPatrolPathManager::*)() const)(&CALifeHumanPatrolPathManager::use_randomness))
			.def("target_game_vertex_id",	&CALifeHumanPatrolPathManager::target_game_vertex_id)
			.def("target_level_vertex_id",	&CALifeHumanPatrolPathManager::target_level_vertex_id)
			.def("target_position",			&CALifeHumanPatrolPathManager::target_level_vertex_id)
	];
}
