////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_obstacles_light_cover_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level obstacles light cover path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelObstaclesLightCoverPathManager CPathManager<\
	CLevelNavigationGraph,\
	_DataStorage,\
	SObstaclesLightCover<\
		_dist_type,\
		_index_type,\
		_iteration_type\
	>,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>

TEMPLATE_SPECIALIZATION
CLevelObstaclesLightCoverPathManager::~CPathManager			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelObstaclesLightCoverPathManager::setup			(
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
	light_weight			= parameters.light_weight;
	cover_weight			= parameters.cover_weight;
	distance_weight			= parameters.distance_weight;
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelObstaclesLightCoverPathManager::evaluate(const _index_type &node_index1, const _index_type &node_index2, const _Graph::const_iterator &i)
{
	VERIFY					(graph);

	const _Graph::CVertex	&tNode1 = *graph->vertex(node_index2);

	y2						= (float)(tNode1.position().y());

	float					cover = 1/(EPS_L + (float)(graph->cover(tNode1)[0])/15.f + (float)(graph->cover(tNode1)[1])/15.f + (float)(graph->cover(tNode1)[2])/15.f  + (float)(graph->cover(tNode1)[3])/15.f);
	float					light = (float)(tNode1.light())/15.f;
	return					(light_weight*light + cover_weight*cover + distance_weight*inherited::evaluate(node_index1,node_index2,i));
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelObstaclesLightCoverPathManager::estimate(const _index_type &node_index) const
{
	VERIFY					(graph);
	return					(inherited::estimate(node_index));
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelObstaclesLightCoverPathManager