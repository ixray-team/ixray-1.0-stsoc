////////////////////////////////////////////////////////////////////////////
//	Module 		: state_gather_items.cpp
//	Created 	: 28.01.2004
//  Modified 	: 28.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Gathering items state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_gather_items.h"
#include "ai/stalker/ai_stalker.h"

using namespace MonsterSpace;

CStateGatherItems::CStateGatherItems	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateGatherItems::~CStateGatherItems	()
{
}

void CStateGatherItems::Init			()
{
}

void CStateGatherItems::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateGatherItems::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateGatherItems::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateGatherItems::initialize		()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskHumming));
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
}

void CStateGatherItems::execute			()
{
	if (!m_object->item())
		return;
	m_object->set_level_dest_vertex	(m_object->item()->level_vertex_id());

	m_object->set_desired_position	(&m_object->item()->Position());
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypePathDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

void CStateGatherItems::finalize		()
{
	inherited::finalize				();
}
