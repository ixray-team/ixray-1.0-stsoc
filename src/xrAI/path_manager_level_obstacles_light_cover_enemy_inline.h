////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_obstacles_light_cover_enemy_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level obstacles light cover enemy path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelObstaclesLightCoverEnemyPathManager CPathManager<\
	CLevelNavigationGraph,\
	_DataStorage,\
	SObstaclesLightCoverEnemy<\
		_dist_type,\
		_index_type,\
		_iteration_type\
	>,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>


TEMPLATE_SPECIALIZATION
CLevelObstaclesLightCoverEnemyPathManager::~CPathManager			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelObstaclesLightCoverEnemyPathManager::setup		(
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
	enemy_position			= parameters.enemy_position;
	enemy_distance			= parameters.enemy_distance;
	enemy_view_weight		= parameters.enemy_view_weight;
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelObstaclesLightCoverEnemyPathManager::evaluate(const _index_type &node_index1, const _index_type &node_index2, const _Graph::const_iterator &i)
{
	VERIFY					(graph);
	return					(enemy_view_weight*_sqr(graph->distance(node_index2,enemy_position) - enemy_distance) + inherited::evaluate(node_index1,node_index2,i));
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelObstaclesLightCoverEnemyPathManager::estimate(const _index_type &node_index) const
{
	VERIFY					(graph);
	return					(inherited::estimate(node_index));
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelObstaclesLightCoverEnemyPathManager