////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker_script.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "motivation_action_manager_stalker.h"
#include "script_space.h"
#include "stalker_decision_space.h"
#include "ai/stalker/ai_stalker_space.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

using namespace luabind;

void CMotivationActionManagerStalker::script_register(lua_State *L)
{
	module(L)
	[
		class_<CMotivationActionManagerStalker>("stalker_ids")
			.enum_("motivations")
			[
				luabind::value("motivation_global",							StalkerDecisionSpace::eMotivationGlobal),
				luabind::value("motivation_alive",							StalkerDecisionSpace::eMotivationAlive),
				luabind::value("motivation_dead",							StalkerDecisionSpace::eMotivationDead),
				luabind::value("motivation_solve_zone_puzzle",				StalkerDecisionSpace::eMotivationSolveZonePuzzle),
				luabind::value("motivation_squad_command",					StalkerDecisionSpace::eMotivationSquadCommand),
				luabind::value("motivation_squad_goal",						StalkerDecisionSpace::eMotivationSquadGoal),
				luabind::value("motivation_squad_action",					StalkerDecisionSpace::eMotivationSquadAction),
				luabind::value("motivation_script",							StalkerDecisionSpace::eMotivationScript)
			]

			.enum_("properties")
			[
				luabind::value("property_alive",							StalkerDecisionSpace::eWorldPropertyAlive),
				luabind::value("property_dead",								StalkerDecisionSpace::eWorldPropertyDead),
				luabind::value("property_already_dead",						StalkerDecisionSpace::eWorldPropertyAlreadyDead),
				luabind::value("property_human_to_dialog",					StalkerDecisionSpace::eWorldPropertyHumanToDialog),
				luabind::value("property_alife",							StalkerDecisionSpace::eWorldPropertyALife),
				luabind::value("property_puzzle_solved",					StalkerDecisionSpace::eWorldPropertyPuzzleSolved),
				luabind::value("property_reached_task_location",			StalkerDecisionSpace::eWorldPropertyReachedTaskLocation),
				luabind::value("property_task_completed",					StalkerDecisionSpace::eWorldPropertyTaskCompleted),
				luabind::value("property_reached_customer_location",		StalkerDecisionSpace::eWorldPropertyReachedCustomerLocation),
				luabind::value("property_customer_satisfied",				StalkerDecisionSpace::eWorldPropertyCustomerSatisfied),
				luabind::value("property_not_enough_food",					StalkerDecisionSpace::eWorldPropertyNotEnoughFood),
				luabind::value("property_can_buy_food",						StalkerDecisionSpace::eWorldPropertyCanBuyFood),
				luabind::value("property_not_enough_medikits",				StalkerDecisionSpace::eWorldPropertyNotEnoughMedikits),
				luabind::value("property_can_buy_medikits",					StalkerDecisionSpace::eWorldPropertyCanBuyMedikit),
				luabind::value("property_no_or_bad_weapon",					StalkerDecisionSpace::eWorldPropertyNoOrBadWeapon),
				luabind::value("property_can_buy_weapon",					StalkerDecisionSpace::eWorldPropertyCanBuyWeapon),
				luabind::value("property_not_enough_ammo",					StalkerDecisionSpace::eWorldPropertyNotEnoughAmmo),
				luabind::value("property_can_buy_ammo",						StalkerDecisionSpace::eWorldPropertyCanBuyAmmo),
				luabind::value("property_items",							StalkerDecisionSpace::eWorldPropertyItems),
				luabind::value("property_enemy",							StalkerDecisionSpace::eWorldPropertyEnemy),
				luabind::value("property_item_to_kill",						StalkerDecisionSpace::eWorldPropertyItemToKill),
				luabind::value("property_found_item_to_kill",				StalkerDecisionSpace::eWorldPropertyFoundItemToKill),
				luabind::value("property_item_can_kill",					StalkerDecisionSpace::eWorldPropertyItemCanKill),
				luabind::value("property_found_ammo",						StalkerDecisionSpace::eWorldPropertyFoundAmmo),
				luabind::value("property_ready_to_kill",					StalkerDecisionSpace::eWorldPropertyReadyToKill),
				luabind::value("property_see_enemy",						StalkerDecisionSpace::eWorldPropertySeeEnemy),
				luabind::value("property_panic",							StalkerDecisionSpace::eWorldPropertyPanic),
				luabind::value("property_in_cover",							StalkerDecisionSpace::eWorldPropertyInCover),
				luabind::value("property_looked_out",						StalkerDecisionSpace::eWorldPropertyLookedOut),
				luabind::value("property_position_holded",					StalkerDecisionSpace::eWorldPropertyPositionHolded),
				luabind::value("property_enemy_detoured",					StalkerDecisionSpace::eWorldPropertyEnemyDetoured),
				luabind::value("property_squad_action",						StalkerDecisionSpace::eWorldPropertySquadAction),
				luabind::value("property_squad_goal",						StalkerDecisionSpace::eWorldPropertySquadAction),
				luabind::value("property_anomaly",							StalkerDecisionSpace::eWorldPropertyAnomaly),
				luabind::value("property_inside_anomaly",					StalkerDecisionSpace::eWorldPropertyInsideAnomaly),
				luabind::value("property_script",							StalkerDecisionSpace::eWorldPropertyScript)
			]
			
			.enum_("action")
			[
				luabind::value("action_already_dead",						StalkerDecisionSpace::eWorldOperatorAlreadyDead),
				luabind::value("action_dead",								StalkerDecisionSpace::eWorldOperatorDead),
				luabind::value("action_gather_items",						StalkerDecisionSpace::eWorldOperatorGatherItems),
				luabind::value("action_no_alife",							StalkerDecisionSpace::eWorldOperatorALifeEmulation),
				luabind::value("action_solve_zone_puzzle",					StalkerDecisionSpace::eWorldOperatorSolveZonePuzzle),
				luabind::value("action_reach_task_location",				StalkerDecisionSpace::eWorldOperatorReachTaskLocation),
				luabind::value("action_accomplish_task",					StalkerDecisionSpace::eWorldOperatorAccomplishTask),
				luabind::value("action_reach_customer_location",			StalkerDecisionSpace::eWorldOperatorReachCustomerLocation),
				luabind::value("action_communicate_with_customer",			StalkerDecisionSpace::eWorldOperatorCommunicateWithCustomer),
				luabind::value("action_dialog",								StalkerDecisionSpace::eWorldOperatorDialog),
				luabind::value("get_out_of_anomaly",						StalkerDecisionSpace::eWorldOperatorGetOutOfAnomaly),
				luabind::value("detect_anomaly",							StalkerDecisionSpace::eWorldOperatorDetectAnomaly),
				luabind::value("action_get_item_to_kill",					StalkerDecisionSpace::eWorldOperatorGetItemToKill),
				luabind::value("action_find_item_to_kill",					StalkerDecisionSpace::eWorldOperatorFindItemToKill),
				luabind::value("action_make_item_killing",					StalkerDecisionSpace::eWorldOperatorMakeItemKilling),
				luabind::value("action_find_ammo",							StalkerDecisionSpace::eWorldOperatorFindAmmo),
				luabind::value("action_aim_enemy",							StalkerDecisionSpace::eWorldOperatorAimEnemy),
				luabind::value("action_get_ready_to_kill",					StalkerDecisionSpace::eWorldOperatorGetReadyToKill),
				luabind::value("action_kill_enemy",							StalkerDecisionSpace::eWorldOperatorKillEnemy),
				luabind::value("action_retreat_from_enemy",					StalkerDecisionSpace::eWorldOperatorRetreatFromEnemy),
				luabind::value("action_take_cover",							StalkerDecisionSpace::eWorldOperatorTakeCover),
				luabind::value("action_look_out",							StalkerDecisionSpace::eWorldOperatorLookOut),
				luabind::value("action_hold_position",						StalkerDecisionSpace::eWorldOperatorHoldPosition),
				luabind::value("action_detour_enemy",						StalkerDecisionSpace::eWorldOperatorDetourEnemy),
				luabind::value("action_search_enemy",						StalkerDecisionSpace::eWorldOperatorSearchEnemy),
				luabind::value("action_squad_action",						StalkerDecisionSpace::eWorldOperatorSquadAction),
				luabind::value("action_death_planner",						StalkerDecisionSpace::eWorldOperatorDeathPlanner),
				luabind::value("action_alife_planner",						StalkerDecisionSpace::eWorldOperatorALifePlanner),
				luabind::value("action_alife_dialog_planner",				StalkerDecisionSpace::eWorldOperatorALifeDialogPlanner),
				luabind::value("action_combat_planner",						StalkerDecisionSpace::eWorldOperatorCombatPlanner),
				luabind::value("action_anomaly_planner",					StalkerDecisionSpace::eWorldOperatorAnomalyPlanner),
				luabind::value("action_script",								StalkerDecisionSpace::eWorldOperatorScript)
			]

			.enum_("sounds")
			[
				luabind::value("sound_die",									StalkerSpace::eStalkerSoundDie),
				luabind::value("sound_die_in_anomaly",						StalkerSpace::eStalkerSoundDieInAnomaly),
				luabind::value("sound_injuring",							StalkerSpace::eStalkerSoundInjuring),
				luabind::value("sound_humming",								StalkerSpace::eStalkerSoundHumming),
				luabind::value("sound_alarm",								StalkerSpace::eStalkerSoundAlarm),
				luabind::value("sound_attack",								StalkerSpace::eStalkerSoundAttack),
				luabind::value("sound_backup",								StalkerSpace::eStalkerSoundBackup),
				luabind::value("sound_detour",								StalkerSpace::eStalkerSoundDetour),
				luabind::value("sound_search",								StalkerSpace::eStalkerSoundSearch),
				luabind::value("sound_injuring_by_friend",					StalkerSpace::eStalkerSoundInjuringByFriend),
				luabind::value("sound_panic_human",							StalkerSpace::eStalkerSoundPanicHuman),
				luabind::value("sound_panic_monster",						StalkerSpace::eStalkerSoundPanicMonster),
				luabind::value("sound_tolls",								StalkerSpace::eStalkerSoundTolls),
				luabind::value("sound_script",								StalkerSpace::eStalkerSoundScript)
			]
		
//		,class_<CAI_Stalker,DLL_Pure>("ce_stalker")
//			.def(constructor<>())
	];
}
