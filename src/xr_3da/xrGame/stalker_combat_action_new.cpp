////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions_new.cpp
//	Created 	: 21.04.2006
//  Modified 	: 21.04.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes new (!?)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_combat_action_new.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "stalker_decision_space.h"
#include "inventory.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "missile.h"
#include "stalker_movement_restriction.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "visual_memory_manager.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"
#include "stalker_planner.h"
#include "agent_member_manager.h"
#include "agent_location_manager.h"
#include "danger_cover_location.h"
#include "ai/stalker/ai_stalker_space.h"
#include "weapon.h"
#include "danger_manager.h"
#include "detail_path_manager.h"

#define DISABLE_COVER_BEFORE_DETOUR

const float TEMP_DANGER_DISTANCE = 5.f;
const u32	TEMP_DANGER_INTERVAL = 120000;

const float	CLOSE_MOVE_DISTANCE	 = 10.f;

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

typedef CStalkerActionBase::_edge_value_type _edge_value_type;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemyNew
//////////////////////////////////////////////////////////////////////////

CStalkerActionRetreatFromEnemyNew::CStalkerActionRetreatFromEnemyNew	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionRetreatFromEnemyNew::initialize	()
{
	inherited::initialize	();

	object().movement().set_movement_type				(eMovementTypeRun);
	object().movement().set_path_type					(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type			(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state					(eBodyStateStand);
	object().movement().set_mental_state				(eMentalStatePanic);
}

void CStalkerActionRetreatFromEnemyNew::finalize	()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionRetreatFromEnemyNew::execute		()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	object().movement().set_nearest_accessible_position	();
}

_edge_value_type CStalkerActionRetreatFromEnemyNew::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return								(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCoverNew
//////////////////////////////////////////////////////////////////////////

CStalkerActionTakeCoverNew::CStalkerActionTakeCoverNew(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionTakeCoverNew::initialize		()
{
	inherited::initialize						();

	m_body_state								= object().movement().body_state();
	m_movement_type								= Random.randI(2) ? eMovementTypeRun : eMovementTypeWalk;

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(m_body_state);
	object().movement().set_movement_type		(m_movement_type);
	object().movement().set_mental_state		(eMentalStateDanger);
	m_storage->set_property						(eWorldPropertyLookedOut,false);
	m_storage->set_property						(eWorldPropertyPositionHolded,false);
	m_storage->set_property						(eWorldPropertyEnemyDetoured,false);

#ifndef SILENT_COMBAT
	if (object().memory().enemy().selected()->human_being()) {
		if (object().agent_manager().member().can_cry_noninfo_phrase()) {
			if (object().memory().visual().visible_now(object().memory().enemy().selected()) && object().agent_manager().member().group_behaviour())
				object().sound().play			(eStalkerSoundBackup,0,0,6000,4000);
//			else
//				object().sound().play			(eStalkerSoundAttack,0,0,6000,4000);
		}
	}
#endif
}

void CStalkerActionTakeCoverNew::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionTakeCoverNew::execute		()
{
	inherited::execute								();

	CMemoryInfo										mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	const CCoverPoint								*point = object().best_cover(mem_object.m_object_params.m_position);
	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		if (object().movement().path_completed() && object().Position().distance_to(point->position()) < 1.f)
			object().brain().affect_cover			(true);
		else
			object().brain().affect_cover			(false);
	}
	else {
		object().movement().set_nearest_accessible_position	();
		object().brain().affect_cover				(true);
	}

	if (fire_make_sense()) {
		u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
		float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
		select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
		object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	}
	else {
		u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
		float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
		select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	}

	if (object().movement().detail().distance_to_target() > CLOSE_MOVE_DISTANCE)
		object().movement().set_body_state			(eBodyStateStand);
	else
		object().movement().set_movement_type		(m_movement_type);

	if (object().movement().path_completed()) {
		object().best_cover_can_try_advance	();
		m_storage->set_property				(eWorldPropertyInCover,true);
	}

	if (object().memory().visual().visible_now(object().memory().enemy().selected()))
		object().sight().setup		(CSightAction(object().memory().enemy().selected(),true,true));
	else
		object().sight().setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillNew
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKillNew::CStalkerActionGetReadyToKillNew	(CAI_Stalker *object, LPCSTR action_name) :
	inherited	(object,action_name)
{
}

void CStalkerActionGetReadyToKillNew::initialize	()
{
	inherited::initialize								();

	m_body_state										= object().movement().body_state();
	m_movement_type										= Random.randI(2) ? eMovementTypeRun : eMovementTypeWalk;

	object().movement().set_desired_direction			(0);
	object().movement().set_path_type					(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type			(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position	();
	object().movement().set_body_state					(m_body_state);
	object().movement().set_movement_type				(eMovementTypeRun);
	object().movement().set_mental_state				(eMentalStateDanger);
	object().sight().setup								(CSightAction(SightManager::eSightTypeCurrentDirection));
//	object().sight().setup								(CSightAction(SightManager::eSightTypePathDirection));
	m_storage->set_property								(eWorldPropertyInCover,false);
	m_storage->set_property								(eWorldPropertyLookedOut,false);
	m_storage->set_property								(eWorldPropertyPositionHolded,false);
	m_storage->set_property								(eWorldPropertyEnemyDetoured,false);
}

void CStalkerActionGetReadyToKillNew::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionGetReadyToKillNew::execute		()
{
	inherited::execute					();

	if (!object().m_best_item_to_kill)
		return;

	if (!object().memory().enemy().selected())
		return;

	if (object().movement().detail().distance_to_target() > CLOSE_MOVE_DISTANCE)
		object().movement().set_body_state		(eBodyStateStand);
	else
		object().movement().set_movement_type	(m_movement_type);

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());
	const CCoverPoint					*point = object().best_cover(mem_object.m_object_params.m_position);
	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		object().movement().set_movement_type		(eMovementTypeRun);
		if (object().movement().path_completed() || object().Position().distance_to(point->position()) < 1.f) {
//			object().movement().set_body_state		(eBodyStateCrouch);
			object().brain().affect_cover			(true);
		}
		else {
//			object().movement().set_body_state		(eBodyStateStand);
			object().brain().affect_cover			(false);
		}
	}
	else {
		object().brain().affect_cover				(true);
		object().movement().set_movement_type		(eMovementTypeStand);
//		object().movement().set_body_state			(eBodyStateCrouch);
		object().movement().set_nearest_accessible_position	();
	}

//	if (object().memory().visual().visible_now(object().memory().enemy().selected()))
//		object().sight().setup	(CSightAction(object().memory().enemy().selected(),true));
//	else
//		object().sight().setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if ((point && !point->position().similar(object().Position(),.5f)) || !object().movement().path_completed())
		object().sight().setup			(CSightAction(SightManager::eSightTypePathDirection));

	u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
	float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
	select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);

	if (object().movement().path_completed())
		object().best_cover_can_try_advance	();
}
