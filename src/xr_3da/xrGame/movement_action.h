////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "control_action.h"

namespace MovementManager {
	enum EPathType;
};

namespace DetailPathManager {
	enum EDetailPathType;
};

namespace MonsterSpace {
	enum EBodyState;
	enum EMovementType;
};

class CAbstractVertexEvaluator;

class CMovementAction : public CControlAction {
protected:
	MovementManager::EPathType			m_path_type;
	DetailPathManager::EDetailPathType	m_detail_path_type;
	MonsterSpace::EBodyState			m_body_state;
	MonsterSpace::EMovementType			m_movement_type;
	u32									m_level_dest_vertex_id;
	Fvector								*m_desired_position;
	Fvector								*m_desired_direction;
	CAbstractVertexEvaluator			*m_node_evaluator;
	CAbstractVertexEvaluator			*m_path_evaluator;

public:
						CMovementAction			();
	IC					CMovementAction			(const MovementManager::EPathType &path_type, const DetailPathManager::EDetailPathType &detail_path_type, const MonsterSpace::EBodyState &body_state, const MonsterSpace::EMovementType &movement_type, u32 level_vertex_id = u32(-1), Fvector *desired_position = 0, Fvector *desired_direction = 0, CAbstractVertexEvaluator *node_evaluator = 0, CAbstractVertexEvaluator *path_evaluator = 0);
			void		execute					();
			void		finalize				();
	IC		void		set_path_type			(const MovementManager::EPathType &path_type);
	IC		void		set_detail_path_type	(const DetailPathManager::EDetailPathType &detail_path_type);
	IC		void		set_body_state			(const MonsterSpace::EBodyState &body_state);
	IC		void		set_movement_type		(const MonsterSpace::EMovementType &movement_type);
	IC		void		set_level_dest_vertex_id(u32 level_vertex_id);
	IC		void		set_desired_position	(Fvector *desired_position);
	IC		void		set_desired_direction	(Fvector *desired_direction);
	IC		void		set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator);
	IC		void		set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator);
};

#include "movement_action_inline.h"