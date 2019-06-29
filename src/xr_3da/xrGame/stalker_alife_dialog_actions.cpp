////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_dialog_actions.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife dialog action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_dialog_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "inventory.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "stalker_decision_space.h"
#include "stalker_animation_manager.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"
#include "ai/stalker/ai_stalker_space.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToDialog
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToDialog::CStalkerActionGetReadyToDialog	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToDialog::initialize	()
{
	inherited::initialize			();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_nearest_accessible_position();
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));

	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		object().CObjectHandler::set_goal		(eObjectActionStrapped,object().best_weapon());
	else
		object().CObjectHandler::set_goal		(eObjectActionIdle);
}

void CStalkerActionGetReadyToDialog::finalize	()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionGetReadyToDialog::execute		()
{
	inherited::execute		();
	if (object().CObjectHandler::weapon_strapped())
		m_storage->set_property	(eWorldPropertyReadyToDialog,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHello
//////////////////////////////////////////////////////////////////////////

CStalkerActionHello::CStalkerActionHello	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionHello::initialize	()
{
	inherited::initialize							();
	object().movement().set_node_evaluator			(0);
	object().movement().set_path_evaluator			(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state				(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().movement().set_nearest_accessible_position();
	object().sight().setup							(SightManager::eSightTypeCurrentDirection);
	object().sound().remove_active_sounds			(u32(eStalkerSoundMaskNoHumming));
	object().body_action							(eBodyActionHello);
	
	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		object().CObjectHandler::set_goal			(eObjectActionStrapped,object().best_weapon());
	else
		object().CObjectHandler::set_goal			(eObjectActionIdle);

	CStalkerAnimationManager						&animation_manager = object().animation();
	animation_manager.setup_storage					(m_storage);
	animation_manager.property_id					(eWorldPropertyHelloCompleted);
	animation_manager.property_value				(true);
}

void CStalkerActionHello::finalize	()
{
	inherited::finalize		();
	object().animation().setup_storage	(0);

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionHello::execute		()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDialog
//////////////////////////////////////////////////////////////////////////

CStalkerActionDialog::CStalkerActionDialog	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDialog::initialize	()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_nearest_accessible_position();
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));
	object().body_action						(eBodyActionHello);
//	
//	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->ID() == object().best_weapon()->ID()))
//		object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
//	else
//		object().CObjectHandler::set_goal	(eObjectActionIdle);
}

void CStalkerActionDialog::finalize	()
{
	inherited::finalize					();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask		(0);
}

void CStalkerActionDialog::execute		()
{
	inherited::execute					();
	object().CObjectHandler::set_goal	(eObjectActionFire1,object().inventory().m_slots[5].m_pIItem);
}
