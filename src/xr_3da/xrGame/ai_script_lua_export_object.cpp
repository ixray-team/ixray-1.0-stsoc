////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_object.cpp
//	Created 	: 19.09.2003
//  Modified 	: 04.05.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "luabind/iterator_policy.hpp"
#include "ai_script_classes.h"
#include "script_binder_object.h"
#include "pdamsg.h"
#include "script_hit.h"
#include "cover_point.h"
#include "script_ini_file.h"
#include "attachable_item.h"

using namespace luabind;

void CScriptEngine::export_object()
{
	module(lua())
	[
		class_<CAbstractVertexEvaluator>("vertex_evaluator"),

		class_<CCoverPoint>("cover_point")
			.def("position",					&CCoverPoint::position)
			.def("level_vertex_id",				&CCoverPoint::level_vertex_id),

		class_<CLuaGameObject>("game_object")
			.enum_("relation")
			[
				value("friend",					int(ALife::eRelationTypeFriend)),
				value("neutral",				int(ALife::eRelationTypeNeutral)),
				value("enemy",					int(ALife::eRelationTypeEnemy)),
				value("dummy",					int(ALife::eRelationTypeDummy))
			]
			.enum_("rank")
			[
				value("novice",					int(ALife::eStalkerRankNovice)),
				value("experienced",			int(ALife::eStalkerRankExperienced)),
				value("veteran",				int(ALife::eStalkerRankVeteran)),
				value("master",					int(ALife::eStalkerRankMaster)),
				value("dummy",					int(ALife::eStalkerRankDummy))
			]
			.enum_("action_types")
			[
				value("movement",				int(CScriptMonster::eActionTypeMovement)),
				value("watch",					int(CScriptMonster::eActionTypeWatch)),
				value("animation",				int(CScriptMonster::eActionTypeAnimation)),
				value("sound",					int(CScriptMonster::eActionTypeSound)),
				value("particle",				int(CScriptMonster::eActionTypeParticle)),
				value("object",					int(CScriptMonster::eActionTypeObject)),
				value("action_type_count",		int(CScriptMonster::eActionTypeCount))
			]
			.enum_("EPathType")
			[
				value("game_path",				int(CMovementManager::ePathTypeGamePath)),
				value("level_path",				int(CMovementManager::ePathTypeLevelPath)),
				value("enemy_search",			int(CMovementManager::ePathTypeEnemySearch)),
				value("patrol_path",			int(CMovementManager::ePathTypePatrolPath)),
				value("no_path",				int(CMovementManager::ePathTypeNoPath))
			]
			
			.property("visible",				&CLuaGameObject::getVisible,		&CLuaGameObject::setVisible)
			.property("enabled",				&CLuaGameObject::getEnabled,		&CLuaGameObject::setEnabled)
			.property("health",					&CLuaGameObject::GetHealth,			&CLuaGameObject::SetHealth)
			.property("power",					&CLuaGameObject::GetPower,			&CLuaGameObject::SetPower)
			.property("satiety",				&CLuaGameObject::GetSatiety,		&CLuaGameObject::SetSatiety)
			.property("radiation",				&CLuaGameObject::GetRadiation,		&CLuaGameObject::SetRadiation)
			.property("circumspection",			&CLuaGameObject::GetCircumspection,	&CLuaGameObject::SetCircumspection)
			.property("morale",					&CLuaGameObject::GetMorale,			&CLuaGameObject::SetMorale)

			.def(								constructor<LPCSTR>())
			.def(								constructor<const CLuaGameObject *>())
			.def("position",					&CLuaGameObject::Position)
			.def("direction",					&CLuaGameObject::Direction)
			.def("clsid",						&CLuaGameObject::clsid)
			.def("id",							&CLuaGameObject::ID)
			.def("section",						&CLuaGameObject::Section)
			.def("name",						&CLuaGameObject::Name)
			.def("parent",						&CLuaGameObject::Parent)
			.def("mass",						&CLuaGameObject::Mass)
			.def("cost",						&CLuaGameObject::Cost)
			.def("death_time",					&CLuaGameObject::DeathTime)
			.def("armor",						&CLuaGameObject::Armor)
			.def("max_health",					&CLuaGameObject::DeathTime)
			.def("accuracy",					&CLuaGameObject::Accuracy)
			.def("alive",						&CLuaGameObject::Alive)
			.def("team",						&CLuaGameObject::Team)
			.def("squad",						&CLuaGameObject::Squad)
			.def("group",						&CLuaGameObject::Group)
			.def("kill",						&CLuaGameObject::Kill)
			.def("hit",							&CLuaGameObject::Hit)
			.def("fov",							&CLuaGameObject::GetFOV)
			.def("range",						&CLuaGameObject::GetRange)
			.def("relation",					&CLuaGameObject::GetRelationType)
			.def("script",						&CLuaGameObject::SetScriptControl)
			.def("get_script",					&CLuaGameObject::GetScriptControl)
			.def("get_script_name",				&CLuaGameObject::GetScriptControlName)
			.def("see",							&CLuaGameObject::CheckObjectVisibility)
			.def("see",							&CLuaGameObject::CheckTypeVisibility)

			.def("who_hit_name",				&CLuaGameObject::WhoHitName)
			.def("who_hit_section_name",		&CLuaGameObject::WhoHitSectionName)
			
			.def("use",							&CLuaGameObject::UseObject)
			.def("rank",						&CLuaGameObject::GetRank)
			.def("command",						CLuaGameObject::AddAction)
			.def("action",						&CLuaGameObject::GetCurrentAction, adopt(result))
			.def("object_count",				&CLuaGameObject::GetInventoryObjectCount)
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(LPCSTR))(CLuaGameObject::GetObjectByName))
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(int))(CLuaGameObject::GetObjectByIndex))
			.def("active_item",					&CLuaGameObject::GetActiveItem)
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::functor<void> &, bool))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::object &, LPCSTR, bool))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::object &, LPCSTR, const CScriptMonster::EActionType))(CLuaGameObject::SetCallback))
			.def("set_callback",				(void (CLuaGameObject::*)(const luabind::functor<void> &, const CScriptMonster::EActionType))(CLuaGameObject::SetCallback))
			.def("clear_callback",				(void (CLuaGameObject::*)(bool))(CLuaGameObject::ClearCallback))
			.def("clear_callback",				(void (CLuaGameObject::*)(const CScriptMonster::EActionType))(CLuaGameObject::ClearCallback))
			.def("patrol",						&CLuaGameObject::GetPatrolPathName)
			.def("set_trade_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetTradeCallback))
			.def("set_trade_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetTradeCallback))			
			.def("clear_trade_callback",		(void (CLuaGameObject::*)())(CLuaGameObject::ClearTradeCallback))
			.def("get_ammo_in_magazine",		&CLuaGameObject::GetAmmoElapsed)
			.def("get_ammo_total",				&CLuaGameObject::GetAmmoCurrent)
			.def("set_queue_size",				&CLuaGameObject::SetQueueSize)
			.def("best_hit",					&CLuaGameObject::GetBestHit)
			.def("best_sound",					&CLuaGameObject::GetBestSound)
			.def("best_enemy",					&CLuaGameObject::GetBestEnemy)
			.def("best_item",					&CLuaGameObject::GetBestItem)
			.def("action_count",				&CLuaGameObject::GetActionCount)
			.def("action_by_index",				&CLuaGameObject::GetActionByIndex)
			.def("set_hit_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetHitCallback))
			.def("set_hit_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetHitCallback))
			.def("clear_hit_callback",			&CLuaGameObject::ClearHitCallback)
			.def("set_hear_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetSoundCallback))
			.def("set_hear_callback",			(void (CLuaGameObject::*)(const luabind::functor<void> &))(CLuaGameObject::SetSoundCallback))
			.def("clear_hear_callback",			&CLuaGameObject::ClearSoundCallback)
			.def("memory",						&CLuaGameObject::memory, adopt(result))
			.def("best_weapon",					&CLuaGameObject::best_weapon)
			.def("explode",						&CLuaGameObject::explode)
			.def("get_enemy",					&CLuaGameObject::GetEnemy)
			.def("get_corpse",					&CLuaGameObject::GetCorpse)
			.def("get_enemy_strength",			&CLuaGameObject::GetEnemyStrength)
			.def("get_sound_info",				&CLuaGameObject::GetSoundInfo)
			.def("get_monster_hit_info",		&CLuaGameObject::GetMonsterHitInfo)
			.def("bind_object",					&CLuaGameObject::bind_object,adopt(_2))
			.def("motivation_action_manager",	&CLuaGameObject::motivation_action_manager)
			.def("set_visible",					&CLuaGameObject::set_visible)

			// movement manager
			.def("set_body_state",				&CLuaGameObject::set_body_state			)
			.def("set_movement_type",			&CLuaGameObject::set_movement_type		)
			.def("set_mental_state",			&CLuaGameObject::set_mental_state		)
			.def("set_path_type",				&CLuaGameObject::set_path_type			)
			.def("set_detail_path_type",		&CLuaGameObject::set_detail_path_type	)
			//
			.def("set_desired_position",		(void (CLuaGameObject::*)())(CLuaGameObject::set_desired_position))
			.def("set_desired_position",		(void (CLuaGameObject::*)(const Fvector *))(CLuaGameObject::set_desired_position))
			.def("set_desired_direction",		(void (CLuaGameObject::*)())(CLuaGameObject::set_desired_direction))
			.def("set_desired_direction",		(void (CLuaGameObject::*)(const Fvector *))(CLuaGameObject::set_desired_direction))
			.def("set_node_evaluator",			(void (CLuaGameObject::*)())(CLuaGameObject::set_node_evaluator))
			.def("set_node_evaluator",			(void (CLuaGameObject::*)(CAbstractVertexEvaluator *))(CLuaGameObject::set_node_evaluator))
			.def("set_path_evaluator",			(void (CLuaGameObject::*)())(CLuaGameObject::set_path_evaluator))
			.def("set_path_evaluator",			(void (CLuaGameObject::*)(CAbstractVertexEvaluator *))(CLuaGameObject::set_path_evaluator))
			.def("set_patrol_path",				&CLuaGameObject::set_patrol_path)
			.def("set_dest_level_vertex_id",	&CLuaGameObject::set_dest_level_vertex_id)
			.def("level_vertex_id",				&CLuaGameObject::level_vertex_id)
			.def("add_animation",				(void (CLuaGameObject::*)(LPCSTR))(CLuaGameObject::add_animation))
			.def("add_animation",				(void (CLuaGameObject::*)(LPCSTR, bool))(CLuaGameObject::add_animation))
			.def("clear_animations",			&CLuaGameObject::clear_animations)
			.def("animation_count",				&CLuaGameObject::animation_count)
			
			// sound_player
			
			.def("add_sound",					(void (CLuaGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32))(CLuaGameObject::add_sound))
			.def("add_sound",					(void (CLuaGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR))(CLuaGameObject::add_sound))
			.def("add_sound",					(void (CLuaGameObject::*)(LPCSTR,u32,ESoundTypes,u32,u32,u32,LPCSTR,LPCSTR))(CLuaGameObject::add_sound))
			.def("remove_sound",				&CLuaGameObject::remove_sound)
			.def("set_sound_mask",				&CLuaGameObject::set_sound_mask)
			.def("play_sound",					(void (CLuaGameObject::*)(u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("play_sound",					(void (CLuaGameObject::*)(u32,u32,u32,u32,u32,u32))(CLuaGameObject::play_sound))
			.def("binded_object",				&CLuaGameObject::binded_object)
			.def("set_previous_point",			&CLuaGameObject::set_previous_point)
			.def("set_start_point",				&CLuaGameObject::set_start_point)
			.def("enable_memory_object",		&CLuaGameObject::enable_memory_object)
			.def("active_sound_count",			&CLuaGameObject::active_sound_count)
			.def("best_cover",					&CLuaGameObject::best_cover)
			.def("spawn_ini",					&CLuaGameObject::spawn_ini)
			.def("memory_visible_objects",		&CLuaGameObject::memory_visible_objects, return_stl_iterator)
			.def("memory_sound_objects",		&CLuaGameObject::memory_sound_objects, return_stl_iterator)
			.def("memory_hit_objects",			&CLuaGameObject::memory_hit_objects, return_stl_iterator)
			.def("not_yet_visible_objects",		&CLuaGameObject::not_yet_visible_objects, return_stl_iterator)
			.def("visibility_threshold",		&CLuaGameObject::visibility_threshold)

			// sight manager
			.def("set_sight",					&CLuaGameObject::set_sight)

			// object handler
			.def("set_item",					(void (CLuaGameObject::*)(MonsterSpace::EObjectAction ))(CLuaGameObject::set_item))
			.def("set_item",					(void (CLuaGameObject::*)(MonsterSpace::EObjectAction, CLuaGameObject *))(CLuaGameObject::set_item))

			.def("bone_position",				&CLuaGameObject::bone_position)

			//////////////////////////////////////////////////////////////////////////
			//inventory owner
			//////////////////////////////////////////////////////////////////////////

			.enum_("EPdaMsg")
			[
				value("trade_pda_msg",			int(ePdaMsgTrade)),
				value("help_pda_msg",			int(ePdaMsgNeedHelp)),
				value("go_away_pda_msg",		int(ePdaMsgGoAway)),
				value("info_pda_msg",			int(ePdaMsgInfo)),
				value("accept_pda_msg",			int(ePdaMsgAccept)),
				value("decline_pda_msg",		int(ePdaMsgDecline)),
				value("i_leave_pda_msg",		int(ePdaMsgILeave)),
				value("no_pda_msg",				int(ePdaMsgMax))
			]

			.def("set_pda_callback",			(void (CLuaGameObject::*)(const luabind::functor<void>&))(CLuaGameObject::SetPdaCallback))
			.def("set_pda_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetPdaCallback))
			.def("set_info_callback",			(void (CLuaGameObject::*)(const luabind::functor<void>&))(CLuaGameObject::SetInfoCallback))
			.def("set_info_callback",			(void (CLuaGameObject::*)(const luabind::object &, LPCSTR))(CLuaGameObject::SetInfoCallback))
			.def("clear_pda_callback",			&CLuaGameObject::ClearPdaCallback)
			.def("clear_info_callback",			&CLuaGameObject::ClearInfoCallback)

			.def("give_info_portion",			&CLuaGameObject::GiveInfoPortion)
			.def("give_info_portion_via_pda",	&CLuaGameObject::GiveInfoPortionViaPda)
			.def("disable_info_portion",		&CLuaGameObject::DisableInfoPortion)
			.def("has_info",					&CLuaGameObject::HasInfo)
			.def("dont_has_info",				&CLuaGameObject::DontHasInfo)

			.def("send_pda_message",			&CLuaGameObject::SendPdaMessage)

			.def("is_talking",					&CLuaGameObject::IsTalking)
			.def("stop_talk",					&CLuaGameObject::StopTalk)
			.def("enable_talk",					&CLuaGameObject::EnableTalk)
			.def("disable_talk",				&CLuaGameObject::DisableTalk)
			.def("is_talk_enabled",				&CLuaGameObject::IsTalkEnabled)

			.def("transfer_item",				&CLuaGameObject::TransferItem)
			.def("transfer_money",				&CLuaGameObject::TransferMoney)
			.def("set_goodwill",				&CLuaGameObject::SetGoodwill)
			.def("set_relation",				&CLuaGameObject::SetRelation)
			.def("set_start_dialog",			&CLuaGameObject::SetStartDialog)


			.enum_("CLSIDS")
			[
				value("no_pda_msg",				int(ePdaMsgMax))
			]
			//HELICOPTER
			.def("air_attack",                  &CLuaGameObject::air_attack)
			.def("air_attack_wait",             &CLuaGameObject::air_attack_wait)
			.def("air_attack_active",           &CLuaGameObject::air_attack_active)

			.def("heli_goto_stay_point",        (void (CLuaGameObject::*)(float))(CLuaGameObject::heli_goto_stay_point))
			.def("heli_goto_stay_point",        (void (CLuaGameObject::*)(Fvector&,float))(CLuaGameObject::heli_goto_stay_point))
			.def("heli_go_patrol",				&CLuaGameObject::heli_go_patrol),

//		class_<CObject>("base_client_class")
//			.def(								constructor<>()),
//
//		class_<CGameObject,CObject>("game_object_class")
//			.def(								constructor<>()),
//
//		class_<CPhysicItem,CGameObject>("inventory_item_class")
//			.def(								constructor<>()),
//
//		class_<CInventoryItem,CPhysicItem>("inventory_item_class")
//			.def(								constructor<>()),
//
//		class_<CGameObject>("game_object_class")
//			.def(								constructor<>()),

//		class_<CAttachableItem,CGameObject>("attachable_item")
		class_<CAttachableItem>("attachable_item")
			.def(								constructor<>())
	];
}
