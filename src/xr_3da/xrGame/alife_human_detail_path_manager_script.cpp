////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human detail path manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_detail_path_manager.h"
#include "script_space.h"
#include "alife_smart_terrain_task.h"

using namespace luabind;

void CALifeHumanDetailPathManager::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CALifeHumanDetailPathManager>("CALifeHumanDetailPathManager")
			.def("target",		(void (CALifeHumanDetailPathManager::*)(const GameGraph::_GRAPH_ID &, const u32 &, const Fvector &))(&CALifeHumanDetailPathManager::target))
			.def("target",		(void (CALifeHumanDetailPathManager::*)(const GameGraph::_GRAPH_ID &))(&CALifeHumanDetailPathManager::target))
			.def("target",		(void (CALifeHumanDetailPathManager::*)(const CALifeSmartTerrainTask *))(&CALifeHumanDetailPathManager::target))
			.def("speed	",		(void (CALifeHumanDetailPathManager::*)(const float &))(&CALifeHumanDetailPathManager::speed))
			.def("speed	",		(const float &(CALifeHumanDetailPathManager::*)() const)(&CALifeHumanDetailPathManager::speed))
			.def("completed",	&CALifeHumanDetailPathManager::completed)
			.def("actual",		&CALifeHumanDetailPathManager::actual)
			.def("failed",		&CALifeHumanDetailPathManager::failed)
	];
}
