////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lua_export_server_entities.cpp
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export server entities
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_space.h"
#include "xrServer_Objects_ALife_All.h"
#include "luabind/adopt_policy.hpp"

using namespace luabind;

void CScriptEngine::export_server_entities()
{
	module(lua())
	[
		class_<CSE_Abstract>("abstract_object")
			.def_readonly("id",				&CSE_Abstract::ID)
			.def_readonly("parent_id",		&CSE_Abstract::ID_Parent),

		class_<CSE_Visual>("visual_server_class"),
		
		class_<CSE_ALifeObject,CSE_Abstract>("alife_object")
			.def(							constructor<LPCSTR>())
			.def("can_switch_online",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_online))
			.def("can_switch_offline",		(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::can_switch_offline))
			.def("interactive",				(bool (CSE_ALifeObject::*)	() const)(CSE_ALifeObject::interactive)),

		class_<CSE_ALifeDynamicObject,CSE_ALifeObject>("alife_dynamic_object")
			.def(							constructor<LPCSTR>()),

		class_<CSE_ALifeDynamicObjectVisual,bases<CSE_ALifeDynamicObject,CSE_Visual> >("alife_dynamic_object_visual")
			.def(							constructor<LPCSTR>()),

		class_<CSE_ALifeInventoryItem>("alife_inventory_item"),

		class_<CSE_ALifeItem,bases<CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem> >("alife_item")
			.def(							constructor<LPCSTR>())
	];
}