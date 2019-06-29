////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_guard_actions.cpp
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker guard action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_guard_actions.h"
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

using namespace StalkerSpace;

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
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeRun);
	object().movement().set_mental_state		(eMentalStateDanger);
}

void CStalkerActionDangerRicochet::finalize					()
{
	inherited::finalize		();
}

void CStalkerActionDangerRicochet::execute					()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	Fvector								position = object().memory().danger().selected()->position();
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}
	else
		object().movement().set_nearest_accessible_position	();

	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		object().CObjectHandler::set_goal		(eObjectActionAimReady1,object().best_weapon());

	object().sight().setup				(CSightAction(SightManager::eSightTypePosition,position,true));

	if (Device.dwTimeGlobal >= object().memory().danger().selected()->time() + 15000)
		object().memory().danger().time_line(Device.dwTimeGlobal);
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
