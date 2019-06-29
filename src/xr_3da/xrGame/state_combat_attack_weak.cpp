////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_attack_weak.cpp
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state attacking weak enemy
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_combat_attack_weak.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory_item.h"
#include "missile.h"

CStateAttackWeak::CStateAttackWeak	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateAttackWeak::~CStateAttackWeak	()
{
}

void CStateAttackWeak::Init			()
{
}

void CStateAttackWeak::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateAttackWeak::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateAttackWeak::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateAttackWeak::initialize		()
{
	inherited::initialize			();
	m_object->set_refresh_rate		(1500);
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
}

void CStateAttackWeak::execute			()
{
	inherited::execute				();
	
	if (!m_object->enemy())
		return;
	
	CMemoryInfo									mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (m_object->visible_now(m_object->enemy())) {
#ifdef OLD_OBJECT_HANDLER
		m_object->CObjectHandler::set_dest_state(eObjectActionFire1,m_object->best_weapon());
#else
		m_object->CObjectHandlerGOAP::set_goal	(eObjectActionFire1,m_object->best_weapon());
#endif
		Fvector						position;
		m_object->enemy()->Center	(position);
		m_object->setup				(SightManager::eSightTypeFirePosition,&position);
	}
	else {
		if (!dynamic_cast<CMissile*>(m_object->best_weapon()))
#ifdef OLD_OBJECT_HANDLER
			m_object->CObjectHandler::set_dest_state(eObjectActionAim1,m_object->best_weapon());
#else
			m_object->CObjectHandlerGOAP::set_goal	(eObjectActionAim1,m_object->best_weapon());
#endif
		
		Fvector			direction;
		direction.sub	(mem_object.m_object_params.m_position,m_object->Position());
		m_object->setup					(SightManager::eSightTypeDirection,&direction);
	}

	if (m_object->visible_now(m_object->enemy()) && (m_object->Position().distance_to(m_object->enemy()->Position()) < 10.f)) {
		m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
		m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
		m_object->set_body_state		(eBodyStateStand);
		m_object->set_movement_type		(eMovementTypeStand);
		m_object->set_mental_state		(eMentalStateDanger);
		return;
	}

	m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
	m_object->set_desired_position	(&mem_object.m_object_params.m_position);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	if (m_object->CMovementManager::path_completed())
		m_object->CMemoryManager::enable		(m_object->enemy(),false);
}

void CStateAttackWeak::finalize		()
{
	inherited::finalize				();
	m_object->set_refresh_rate		(0);
}
