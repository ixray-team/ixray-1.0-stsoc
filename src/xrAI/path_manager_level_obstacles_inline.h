////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_obstacles_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level obstacles path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelObstaclesPathManager CPathManager<\
	CLevelNavigationGraph,\
	_DataStorage,\
	SObstacleParams<\
		_dist_type,\
		_index_type,\
		_iteration_type\
	>,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>

TEMPLATE_SPECIALIZATION
CLevelObstaclesPathManager::~CPathManager			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelObstaclesPathManager::setup			(
			const _Graph			*_graph,
			_DataStorage			*_data_storage,
			xr_vector<_index_type>	*_path,
			const _index_type		&_start_node_index,
			const _index_type		&_goal_node_index,
			const _Parameters		&parameters
		)
{
	inherited::setup(
		_graph,
		_data_storage,
		_path,
		_start_node_index,
		_goal_node_index,
		parameters
	);
	m_avoid_dynamic_obstacles		= parameters.avoid_dynamic_obstacles;
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelObstaclesPathManager::is_accessible	(const _index_type &vertex_id) const
{
	VERIFY					(graph);
	return					(graph->valid_vertex_id(vertex_id) && (!m_avoid_dynamic_obstacles || graph->is_accessible(vertex_id)));
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelObstaclesPathManager