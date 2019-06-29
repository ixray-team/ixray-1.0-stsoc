////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_enemy.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for enemy searches
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "enemy_location_predictor.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"

void CMovementManager::process_enemy_search()
{
	switch (m_path_state) {
		case ePathStatePredictEnemyVertices : {
			enemy_location_predictor().predict_enemy_locations();
			if (enemy_location_predictor().prediction_failed())
				break;
			m_path_state		= ePathStateSelectEnemyVertex;
			if (time_over())
				break;
		}
		case ePathStateSelectEnemyVertex : {
			enemy_location_predictor().select_enemy_location();
			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())
				break;
		}
		case ePathStateBuildLevelPath : {
			level_path_manager().build_path(object().ai_location().level_vertex_id(),level_dest_vertex_id());
			if (level_path_manager().failed())
				break;
			m_path_state		= ePathStateContinueLevelPath;
			if (time_over())
				break;
		}
		case ePathStateContinueLevelPath : {
			level_path_manager().select_intermediate_vertex();
			m_path_state		= ePathStateBuildDetailPath;
			if (time_over())
				break;
		}
		case ePathStateBuildDetailPath : {
			detail_path_manager().set_state_patrol_path(true);
			detail_path_manager().set_start_position(object().Position());
			detail_path_manager().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
			detail_path_manager().set_dest_position(
				ai().level_graph().vertex_position(
					level_path_manager().intermediate_vertex_id()
				)
			);
			detail_path_manager().build_path	(
				level_path_manager().path(),
				level_path_manager().intermediate_index()
			);
			
			if (detail_path_manager().failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				break;
			}
			m_path_state		= ePathStatePathVerification;
			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!enemy_location_predictor().enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			else
			if (!level_path_manager().actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!detail_path_manager().actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (detail_path_manager().completed(object().Position(),!detail_path_manager().state_patrol_path())) {
					m_path_state	= ePathStateContinueLevelPath;
					if (level_path_manager().completed())
						m_path_state	= ePathStateContinueGamePath;
				}
			break;
		}
		case ePathStatePathCompleted : {
			if (!enemy_location_predictor().enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			break;
		}
		default : NODEFAULT;
	}
}
