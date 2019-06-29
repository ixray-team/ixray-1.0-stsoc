////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_path_manager.h"
#include "ai_space.h"
#include "graph_search_engine.h"

CLevelPathManager::CLevelPathManager	()
{
	init					();
}

CLevelPathManager::~CLevelPathManager	()
{
}

void CLevelPathManager::init			()
{
	m_level_path_actual		= true;
	m_level_start_node		= u32(-1);
	m_level_dest_node		= u32(-1);
	m_level_cur_node_index	= u32(-1);
	m_level_path_type		= eLevelPathTypeMinTime;
	m_max_range				= dInfinity;
	m_max_iterations		= u32(-1);
	m_max_node_visited		= u32(-1);
	m_consider_obstacles	= true;
	m_cover_weight			= 20;
	m_light_weight			= 10;
	m_distance_weight		= 40;
	m_enemy_position		= Fvector().set(0,0,0);
	m_enemy_distance		= 30;
	m_enemy_view			= 20;
}

bool CLevelPathManager::valid			() const
{
	return				(!m_level_path.empty() && (m_level_path[0] == m_level_start_node) && (m_level_path[m_level_path.size() - 1] == m_level_dest_node));
}

bool CLevelPathManager::valid			(u32 node_id) const
{
	return				(node_id < ai().game_graph().header().vertex_count());
}

void CLevelPathManager::build_level_path()
{
	if (!m_level_path_actual && valid(m_level_start_node) && valid(m_level_dest_node)) {
		switch (m_level_path_type) {
			case eLevelPathTypeMinTime : {
				m_level_path_actual		= ai().graph_search_engine().build_path(
					ai().level_graph(),
					m_level_start_node,
					m_level_dest_node,
					&m_level_path,
					CGraphSearchEngine::CObstacleParams(
						m_max_range,
						m_max_iterations,
						m_max_node_visited,
						m_consider_obstacles
					)
				);
				break;
			}
			case eLevelPathTypePositionDetour : {
				m_level_path_actual		= ai().graph_search_engine().build_path(
					ai().level_graph(),
					m_level_start_node,
					m_level_dest_node,
					&m_level_path,
					CGraphSearchEngine::CObstaclesLightCover(
						m_max_range,
						m_max_iterations,
						m_max_node_visited,
						m_consider_obstacles,
						m_cover_weight,
						m_light_weight,
						m_distance_weight
					)
				);
				break;
			}
			case eLevelPathTypeCoverLight : {
				m_level_path_actual		= ai().graph_search_engine().build_path(
					ai().level_graph(),
					m_level_start_node,
					m_level_dest_node,
					&m_level_path,
					CGraphSearchEngine::CObstaclesLightCoverEnemy(
						m_max_range,
						m_max_iterations,
						m_max_node_visited,
						m_consider_obstacles,
						m_cover_weight,
						m_light_weight,
						m_distance_weight,
						m_enemy_position,
						m_enemy_distance,
						m_enemy_view
					)
				);
				break;
			}
		}
		if (m_level_path_actual)
			m_level_cur_node_index = 0;
	}
}

void CLevelPathManager::select_level_path_vertex()
{
}

bool CLevelPathManager::level_path_actual() const
{
	return				(true);
}

bool CLevelPathManager::level_path_completed() const
{
	return				(true);
}
