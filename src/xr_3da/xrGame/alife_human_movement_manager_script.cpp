////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_movement_manager_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human movement manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_movement_manager.h"
#include "alife_human_detail_path_manager.h"
#include "alife_human_patrol_path_manager.h"
#include "script_space.h"

using namespace luabind;

CALifeHumanDetailPathManager *get_detail(const CALifeHumanMovementManager *self)
{
	return	(&self->detail());
}

CALifeHumanPatrolPathManager *get_patrol(const CALifeHumanMovementManager *self)
{
	return	(&self->patrol());
}

void CALifeHumanMovementManager::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CALifeHumanMovementManager>("CALifeHumanMovementManager")
			.def("detail",		&get_detail)
			.def("patrol",		&get_patrol)
			.def("path_type",	(void (CALifeHumanMovementManager::*)(const EPathType &))(&CALifeHumanMovementManager::path_type))
			.def("path_type",	(const EPathType & (CALifeHumanMovementManager::*)() const)(&CALifeHumanMovementManager::path_type))
			.def("actual",		&CALifeHumanMovementManager::actual)
			.def("completed",	&CALifeHumanMovementManager::completed)
	];
}
