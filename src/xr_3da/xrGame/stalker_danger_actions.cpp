////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_dangers_actions.cpp
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_actions.h"
#include "script_game_object.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"
#include "sight_manager.h"
#include "object_handler.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "sight_manager_space.h"
#include "ai/stalker/ai_stalker_space.h"
#include "memory_manager.h"
#include "danger_manager.h"
#include "ai_space.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "cover_evaluators.h"
#include "stalker_movement_restriction.h"
#include "inventory.h"
#include "agent_member_manager.h"
#include "danger_cover_location.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

// temporary copy-paste "technique" :-(
const float DANGER_DISTANCE = 5.f;
const u32	DANGER_INTERVAL = 120000;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerRicochet
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerRicochet::CStalkerActionDangerRicochet	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerRicochet::initialize				()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	m_cover_selection_position					= object().Position();
}

void CStalkerActionDangerRicochet::finalize					()
{
	inherited::finalize		();
}

void CStalkerActionDangerRicochet::execute					()
{
	inherited::execute					();

	if (!object().memory().danger().selected())
		return;

	if (object().m_ce_best->selected() && !object().agent_manager().member().member(m_object).cover())
		object().m_ce_best->invalidate	();

	if (object().movement().path_completed())
		m_cover_selection_position		= object().Position();

	CCoverPoint							*point, *last_cover = object().agent_manager().member().member(m_object).cover();
	Fvector								position = object().memory().danger().selected()->position();
	for (;;) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(m_cover_selection_position,10.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true,false));
		if (!point) {
			object().m_ce_best->setup	(position,10.f,170.f,10.f);
			point						= ai().cover_manager().best_cover(m_cover_selection_position,30.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true,false));
		}

		if (point == last_cover)
			break;

		if (last_cover && (point->position().distance_to_sqr(last_cover->position()) <= 1.f)) {
			point						= last_cover;
			break;
		}

		if (m_cover_selection_position.similar(object().Position()))
			break;

		m_cover_selection_position		= object().Position();
	}

	object().agent_manager().location().make_suitable(m_object,point);

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}	
	else
		object().movement().set_nearest_accessible_position	();

	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());

	if (!object().movement().path_completed()) {
		object().movement().set_body_state		(eBodyStateStand);
		object().movement().set_movement_type	(eMovementTypeRun);
		set_inertia_time						(int(Device.dwTimeGlobal - start_level_time()) + 1500000);
		object().sight().setup					(CSightAction(SightManager::eSightTypeCover,true,true));
		return;
	}

	set_property						(eWorldPropertyInCover,	true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerLookAround
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerLookAround::CStalkerActionDangerLookAround	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerLookAround::initialize				()
{
	set_inertia_time							(15000);
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateCrouch);
}

void CStalkerActionDangerLookAround::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerLookAround::execute				()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	object().sight().setup						(CSightAction(SightManager::eSightTypeCoverLookOver,true));

	if (completed()) {
		object().agent_manager().location().add		(
			xr_new<CDangerCoverLocation>(
				object().agent_manager().member().member(&object()).cover(),
				Device.dwTimeGlobal,
				DANGER_INTERVAL,
				DANGER_DISTANCE
			)
		);
	}
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerSearch
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerSearch::CStalkerActionDangerSearch	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerSearch::initialize				()
{
	set_inertia_time							(300000);
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateCrouch);
}

void CStalkerActionDangerSearch::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerSearch::execute				()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	object().agent_manager().location().add		(
		xr_new<CDangerCoverLocation>(
			object().agent_manager().member().member(&object()).cover(),
			Device.dwTimeGlobal,
			DANGER_INTERVAL,
			DANGER_DISTANCE
		)
	);
//	if (completed())
//		object().memory().danger().time_line(Device.dwTimeGlobal);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerShot
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerShot::CStalkerActionDangerShot	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerShot::initialize			()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerShot::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerShot::execute				()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerHit
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerHit::CStalkerActionDangerHit	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerHit::initialize			()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerHit::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerHit::execute				()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerDeath
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerDeath::CStalkerActionDangerDeath	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerDeath::initialize				()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerDeath::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerDeath::execute					()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerAttack
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerAttack::CStalkerActionDangerAttack	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerAttack::initialize				()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerAttack::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerAttack::execute					()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerCorpse
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerCorpse::CStalkerActionDangerCorpse	(CAI_Stalker *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CStalkerActionDangerCorpse::initialize				()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerCorpse::finalize				()
{
	inherited::finalize		();
}

void CStalkerActionDangerCorpse::execute					()
{
	inherited::execute		();
}
