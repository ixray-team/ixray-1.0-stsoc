////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_action.h"
#include "ai/stalker/ai_stalker.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "stalker_movement_manager.h"

CMovementAction::CMovementAction	()
{
	m_path_type					= MovementManager::ePathTypeNoPath;
	m_detail_path_type			= DetailPathManager::eDetailPathTypeSmooth;
	m_body_state				= MonsterSpace::eBodyStateStand;
	m_movement_type				= MonsterSpace::eMovementTypeStand;
	m_level_dest_vertex_id		= u32(-1);
	m_desired_position			= 0;
	m_desired_direction			= 0;
	m_node_evaluator			= 0;
	m_path_evaluator			= 0;
}

void CMovementAction::execute		()
{
	object().movement().set_path_type			(m_path_type);
	object().movement().set_detail_path_type	(m_detail_path_type);
	object().movement().set_body_state			(m_body_state);
	object().movement().set_movement_type		(m_movement_type);
	object().movement().set_level_dest_vertex	(m_level_dest_vertex_id);
	object().movement().set_desired_position	(m_desired_position);
	object().movement().set_desired_direction	(m_desired_direction);
	object().movement().set_node_evaluator		(m_node_evaluator);
	object().movement().set_node_evaluator		(m_path_evaluator);
}

void CMovementAction::finalize	()
{
	object().movement().set_desired_position	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_node_evaluator	(0);
	object().movement().set_node_evaluator	(0);
}
