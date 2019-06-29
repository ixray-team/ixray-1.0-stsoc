////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_effector.h"
#include "../cameramanager.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "ParticlesObject.h"
#include "ArtifactMerger.h"
#include "actor.h"
#include "ai_script_classes.h"
#include "ai/stalker/ai_stalker.h"
#include "script_binder_object.h"
#include "pdamsg.h"
#include "object_factory.h"

using namespace luabind;

CRenderDevice *get_device()
{
	return		(&Device);
}

CLuaGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return	(dynamic_cast<CGameObject*>(l_tpActor)->lua_game_object());
	else
		return	(0);
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}

//CLuaGameObject *get_object_by_id(ALife::_OBJECT_ID id)
//{
//	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByID(id));
//	if (l_tpGameObject)
//		return		(l_tpGameObject->lua_game_object());
//	else
//		return		(0);
//}
//
LPCSTR get_weather	()
{
	return			(*g_pGamePersistent->Environment.GetWeather());
}

void set_weather	(LPCSTR weather_name)
{
	VERIFY3			(g_pGamePersistent->Environment.Weathers.find(weather_name) != g_pGamePersistent->Environment.Weathers.end(),"There is no weather with the specified name ",weather_name);
	return			(g_pGamePersistent->Environment.SetWeather(weather_name));
}

void set_time_factor(float time_factor)
{
	Level().SetGameTimeFactor(time_factor);
}

float get_time_factor()
{
	return			(Level().GetGameTimeFactor());
}

float cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y,p;
	direction.getHP	(y,p);
	return			(ai().level_graph().cover_in_direction(y,level_vertex_id));
}

float rain_factor()
{
	return			(g_pGamePersistent->Environment.CurrentEnv.rain_density);
}

u32	vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance)
{
	direction.normalize_safe();
	direction.mul	(max_distance);
	Fvector			start_position = ai().level_graph().vertex_position(level_vertex_id);
	Fvector			finish_position = Fvector(start_position).add(direction);
	u32				result = u32(-1);
	ai().level_graph().find_farthest_node_in_direction(level_vertex_id,start_position,finish_position,result,0);
	return			(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

CLuaGameObject *not_yet_visible_object(const MemorySpace::CNotYetVisibleObject &object)
{
	return			(object.m_object->lua_game_object());
}

Fvector vertex_position(u32 level_vertex_id)
{
	return			(ai().level_graph().vertex_position(level_vertex_id));
}

void map_add_object_icon(CLuaGameObject* lua_object, u16 text_id)
{
	Level().AddObjectMapLocation(lua_object->object());
}
void map_remove_object_icon(CLuaGameObject* lua_object)
{
	Level().RemoveMapLocationByID(lua_object->object()->ID());
}

void CScriptEngine::export_artifact_merger()
{
	module(lua())
	[
		class_<CArtifactMerger>("artifact_merger")
			.def("get_mercury_ball_num",	&CArtifactMerger::GetMercuryBallNum)
			.def("get_gravi_num",			&CArtifactMerger::GetGraviArtifactNum)
			.def("get_black_drops_num",		&CArtifactMerger::GetBlackDropsNum)
			.def("get_needles_num",			&CArtifactMerger::GetNeedlesNum)

			.def("destroy_mercury_ball",	&CArtifactMerger::DestroyMercuryBall)
			.def("destroy_gravi",			&CArtifactMerger::DestroyGraviArtifact)
			.def("destroy_black_drops",		&CArtifactMerger::DestroyBlackDrops)
			.def("destroy_needles",			&CArtifactMerger::DestroyNeedles)

			.def("spawn_mercury_ball",	&CArtifactMerger::SpawnMercuryBall)
			.def("spawn_gravi",			&CArtifactMerger::SpawnGraviArtifact)
			.def("spawn_black_drops",	&CArtifactMerger::SpawnBlackDrops)
			.def("spawn_needles",		&CArtifactMerger::SpawnNeedles)
	];
}

void CScriptEngine::export_effector()
{
	module(lua())
		[
			class_<SPPInfo::SDuality>("duality")
			.def_readwrite("h",					&SPPInfo::SDuality::h)
			.def_readwrite("v",					&SPPInfo::SDuality::v)
			.def(								constructor<>()),

			class_<SPPInfo::SColor>	("color")
			.def_readwrite("r",					&SPPInfo::SColor::r)
			.def_readwrite("g",					&SPPInfo::SColor::g)
			.def_readwrite("b",					&SPPInfo::SColor::b)
			.def(								constructor<>()),

		class_<SPPInfo::SNoise>("noise")
			.def_readwrite("intensity",			&SPPInfo::SNoise::intensity)
			.def_readwrite("grain",				&SPPInfo::SNoise::grain)
			.def_readwrite("fps",				&SPPInfo::SNoise::fps)
			.def(								constructor<>()),

		class_<SPPInfo>("effector_params")
			.def_readwrite("blur",				&SPPInfo::blur)
			.def_readwrite("gray",				&SPPInfo::gray)
			.def_readwrite("dual",				&SPPInfo::duality)
			.def_readwrite("noise",				&SPPInfo::noise)
			.def_readwrite("color_base",		&SPPInfo::color_base)
			.def_readwrite("color_gray",		&SPPInfo::color_gray)
			.def_readwrite("color_add",			&SPPInfo::color_add)
			.def(								constructor<>()),

		class_<CLuaEffector, CLuaEffectorWrapper>("effector")
			.def_readwrite("info",				&CLuaEffector::m_tInfo)
			.def(								constructor<int,float>())
			.def("start",						&CLuaEffector::Add)
			.def("finish",						&CLuaEffector::Remove)
			.def("process",						&CLuaEffector::Process,	&CLuaEffectorWrapper::Process_static)
	];
}

bool patrol_path_exists(LPCSTR patrol_path)
{
	return		(!!Level().patrol_paths().path(patrol_path,true));
}

void CScriptEngine::export_level()
{
	module(lua(),"level")
	[
		// declarations
		def("object",							get_object_by_name),
//		def("object",							get_object_by_id),
		def("actor",							tpfGetActor),
		//def("set_artifact_merge",				SetArtifactMergeFunctor),
		def("get_weather",						get_weather),
		def("set_weather",						set_weather),
		def("set_time_factor",					set_time_factor),
		def("get_time_factor",					get_time_factor),
		def("cover_in_direction",				cover_in_direction),
		def("vertex_in_direction",				vertex_in_direction),
		def("rain_factor",						rain_factor),
		def("patrol_path_exists",				patrol_path_exists),
		def("vertex_position",					vertex_position),

		def("map_add_object_icon",				map_add_object_icon),
		def("map_remove_object_icon",			map_remove_object_icon)
	];

	module(lua())
	[
		def("device",							get_device)
	];
}

void CScriptEngine::export_particles()
{
	module(lua())
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR,bool>())
			.def("position",					&CParticlesObject::Position)
			.def("play_at_pos",					&CParticlesObject::play_at_pos)
			.def("stop",						&CParticlesObject::Stop)
	];
}

int get_sound_type(const CSoundObject &sound_object)
{
	return			((int)sound_object.m_sound_type);
}

int bit_and(int i, int j)
{
	return			(i & j);
}

int bit_or(int i, int j)
{
	return			(i | j);
}

int bit_xor(int i, int j)
{
	return			(i ^ j);
}

int bit_not(int i)
{
	return			(~i);
}

struct SSoundType{};

template <typename T>
CLuaGameObject *get_memory_object(const MemorySpace::CMemoryObject<T> &memory_object)
{
	return			(memory_object.m_object->lua_game_object());
}

void CScriptEngine::export_memory_objects()
{
	module(lua())
	[
		class_<SRotation>("rotation")
			.def_readonly("yaw",			&SRotation::yaw)
			.def_readonly("pitch",			&SRotation::pitch),
			
		class_<MemorySpace::SObjectParams>("object_params")
			.def_readonly("level_vertex",	&MemorySpace::SObjectParams::m_level_vertex_id)
			.def_readonly("position",		&MemorySpace::SObjectParams::m_position)
			.def_readonly("orientation",	&MemorySpace::SObjectParams::m_orientation),
			
		class_<MemorySpace::SMemoryObject>("memory_object")
			.def_readonly("game_time",		&MemorySpace::SMemoryObject::m_game_time)
			.def_readonly("level_time",		&MemorySpace::SMemoryObject::m_level_time)
			.def_readonly("last_game_time",	&MemorySpace::SMemoryObject::m_last_game_time)
			.def_readonly("last_level_time",&MemorySpace::SMemoryObject::m_last_level_time)
			.def_readonly("first_game_time",&MemorySpace::SMemoryObject::m_first_game_time)
			.def_readonly("first_level_time",&MemorySpace::SMemoryObject::m_first_level_time)
			.def_readonly("update_count",	&MemorySpace::SMemoryObject::m_update_count),

		class_<MemorySpace::CMemoryObject<CEntityAlive>,MemorySpace::SMemoryObject>("entity_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CEntityAlive>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CEntityAlive>::m_self_params)
			.def("object",					&get_memory_object<CEntityAlive>),

		class_<MemorySpace::CMemoryObject<CGameObject>,MemorySpace::SMemoryObject>("game_memory_object")
			.def_readonly("object_info",	&MemorySpace::CMemoryObject<CGameObject>::m_object_params)
			.def_readonly("self_info",		&MemorySpace::CMemoryObject<CGameObject>::m_self_params)
			.def("object",					&get_memory_object<CGameObject>),

		class_<MemorySpace::CHitObject,MemorySpace::CMemoryObject<CEntityAlive> >("hit_memory_object")
			.def_readonly("direction",		&MemorySpace::CHitObject::m_direction)
			.def_readonly("bone_index",		&MemorySpace::CHitObject::m_bone_index)
			.def_readonly("amount",			&MemorySpace::CHitObject::m_amount),
		
		class_<MemorySpace::CVisibleObject,MemorySpace::CMemoryObject<CGameObject> >("visible_memory_object")
			.def_readonly("visible",		&MemorySpace::CSoundObject::m_visible),

		class_<MemorySpace::CMemoryInfo,MemorySpace::CVisibleObject>("memory_info")
			.def_readonly("visual_info",	&MemorySpace::CMemoryInfo::m_visual_info)
			.def_readonly("sound_info",		&MemorySpace::CMemoryInfo::m_sound_info)
			.def_readonly("hit_info",		&MemorySpace::CMemoryInfo::m_hit_info),

		class_<SSoundType>("snd_type")
			.enum_("sound_types")
			[
				value("no_sound",				int(SOUND_TYPE_NO_SOUND					)),
				value("weapon",					int(SOUND_TYPE_WEAPON					)),
				value("item",					int(SOUND_TYPE_ITEM						)),
				value("monster",				int(SOUND_TYPE_MONSTER					)),
				value("anomaly",				int(SOUND_TYPE_ANOMALY					)),
				value("world",					int(SOUND_TYPE_WORLD					)),
				value("pick_up",				int(SOUND_TYPE_PICKING_UP				)),
				value("drop",					int(SOUND_TYPE_DROPPING					)),
				value("hide",					int(SOUND_TYPE_HIDING					)),
				value("take",					int(SOUND_TYPE_TAKING					)),
				value("use",					int(SOUND_TYPE_USING					)),
				value("shoot",					int(SOUND_TYPE_SHOOTING					)),
				value("empty",					int(SOUND_TYPE_EMPTY_CLICKING			)),
				value("bullet_hit",				int(SOUND_TYPE_BULLET_HIT				)),
				value("reload",					int(SOUND_TYPE_RECHARGING				)),
				value("die",					int(SOUND_TYPE_DYING					)),
				value("injure",					int(SOUND_TYPE_INJURING					)),
				value("step",					int(SOUND_TYPE_STEP						)),
				value("talk",					int(SOUND_TYPE_TALKING					)),
				value("attack",					int(SOUND_TYPE_ATTACKING				)),
				value("eat",					int(SOUND_TYPE_EATING					)),
				value("idle",					int(SOUND_TYPE_IDLE						)),
				value("object_break",			int(SOUND_TYPE_OBJECT_BREAKING			)),
				value("object_collide",			int(SOUND_TYPE_OBJECT_COLLIDING			)),
				value("object_explode",			int(SOUND_TYPE_OBJECT_EXPLODING			)),
				value("ambient",				int(SOUND_TYPE_AMBIENT					)),
				value("item_pick_up",			int(SOUND_TYPE_ITEM_PICKING_UP			)),
				value("item_drop",				int(SOUND_TYPE_ITEM_DROPPING			)),
				value("item_hide",				int(SOUND_TYPE_ITEM_HIDING				)),
				value("item_take",				int(SOUND_TYPE_ITEM_TAKING				)),
				value("item_use",				int(SOUND_TYPE_ITEM_USING				)),
				value("weapon_shoot",			int(SOUND_TYPE_WEAPON_SHOOTING			)),
				value("weapon_empty",			int(SOUND_TYPE_WEAPON_EMPTY_CLICKING	)),
				value("weapon_bullet_hit",		int(SOUND_TYPE_WEAPON_BULLET_HIT		)),
				value("weapon_reload",			int(SOUND_TYPE_WEAPON_RECHARGING		)),
				value("monster_die",			int(SOUND_TYPE_MONSTER_DYING			)),
				value("monster_injure",			int(SOUND_TYPE_MONSTER_INJURING			)),
				value("monster_step",			int(SOUND_TYPE_MONSTER_STEP				)),
				value("monster_talk",			int(SOUND_TYPE_MONSTER_TALKING			)),
				value("monster_attack",			int(SOUND_TYPE_MONSTER_ATTACKING		)),
				value("monster_eat",			int(SOUND_TYPE_MONSTER_EATING			)),
				value("anomaly_idle",			int(SOUND_TYPE_ANOMALY_IDLE				)),
				value("world_object_break",		int(SOUND_TYPE_WORLD_OBJECT_BREAKING	)),
				value("world_object_collide",	int(SOUND_TYPE_WORLD_OBJECT_COLLIDING	)),
				value("world_object_explode",	int(SOUND_TYPE_WORLD_OBJECT_EXPLODING	)),
				value("world_ambient",			int(SOUND_TYPE_WORLD_AMBIENT			))
			],

		class_<MemorySpace::CSoundObject,MemorySpace::CVisibleObject>("sound_memory_object")
			.def("type",					&MemorySpace::CSoundObject::sound_type)
			.def_readonly("power",			&MemorySpace::CSoundObject::m_power),

		def("bit_and",						bit_and),
		def("bit_or",						bit_or),
		def("bit_xor",						bit_xor),
		def("bit_not",						bit_not),

		class_<MemorySpace::CNotYetVisibleObject>("not_yet_visible_object")
			.def_readonly("value",			&MemorySpace::CNotYetVisibleObject::m_value)
			.def("object",					&not_yet_visible_object),

		class_<CObjectFactory>("object_factory")
			.def("register",				&CObjectFactory::register_script_class)
			.def("set_instance",			(void (CObjectFactory::*)(CObjectFactory::CLIENT_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
			.def("set_instance",			(void (CObjectFactory::*)(CObjectFactory::SERVER_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
	];
}
