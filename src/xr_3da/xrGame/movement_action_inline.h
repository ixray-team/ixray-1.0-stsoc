////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CMovementAction::CMovementAction				(const MovementManager::EPathType &path_type, const DetailPathManager::EDetailPathType &detail_path_type, const MonsterSpace::EBodyState &body_state, const MonsterSpace::EMovementType &movement_type, u32 level_vertex_id, Fvector *desired_position, Fvector *desired_direction, CAbstractVertexEvaluator *node_evaluator, CAbstractVertexEvaluator *path_evaluator)
{
	m_path_type				= path_type;
	m_detail_path_type		= detail_path_type;
	m_body_state			= body_state;
	m_movement_type			= movement_type;
	m_level_dest_vertex_id	= level_vertex_id;
	m_desired_position		= desired_position;
	m_desired_direction		= desired_direction;
	m_node_evaluator		= node_evaluator;
	m_path_evaluator		= path_evaluator;
}

IC	void CMovementAction::set_path_type				(const MovementManager::EPathType &path_type)
{
	m_path_type				= path_type;
}

IC	void CMovementAction::set_detail_path_type		(const DetailPathManager::EDetailPathType &detail_path_type)
{
	m_detail_path_type		= detail_path_type;
}

IC	void CMovementAction::set_body_state			(const MonsterSpace::EBodyState &body_state)
{
	m_body_state			= body_state;
}

IC	void CMovementAction::set_movement_type			(const MonsterSpace::EMovementType &movement_type)
{
	m_movement_type			= movement_type;
}

IC	void CMovementAction::set_level_dest_vertex_id	(u32 level_vertex_id)
{
	m_level_dest_vertex_id	= level_vertex_id;
}

IC	void CMovementAction::set_desired_position		(Fvector *desired_position)
{
	m_desired_position		= desired_position;
}

IC	void CMovementAction::set_desired_direction		(Fvector *desired_direction)
{
	m_desired_direction		= desired_direction;
}

IC	void CMovementAction::set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator)
{
	m_node_evaluator		= node_evaluator;
}

IC	void CMovementAction::set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator)
{
	m_path_evaluator		= path_evaluator;
}
