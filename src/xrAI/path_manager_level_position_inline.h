////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_position_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level position path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelPositionPathManager CPathManager<\
	CLevelNavigationGraph,\
	_DataStorage,\
	SPosition<\
		_dist_type,\
		_index_type,\
		_iteration_type\
	>,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>


TEMPLATE_SPECIALIZATION
CLevelPositionPathManager::~CPathManager		()
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelPositionPathManager::setup			(
		const _Graph			*_graph,
		_DataStorage			*_data_storage,
		xr_vector<_index_type>	*_path,
		const _index_type		&_start_node_index,
		const _index_type		&_goal_node_index,
		_Parameters				&parameters
	)
{
	inherited::setup(
		_graph,
		_data_storage,
		_path,
		_start_node_index,
		_start_node_index,
		parameters
	);
	m_params				= &parameters;
	m_params->m_vertex_id	= _index_type(-1);
	m_params->m_distance	= m_params->max_range;
}

TEMPLATE_SPECIALIZATION
IC	void CLevelPositionPathManager::init			()
{
	inherited::init			();
	graph->unpack_xz		(graph->vertex_position(m_params->m_position),x3,z3);
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelPositionPathManager::is_goal_reached	(const _index_type &node_index)
{
	best_node				= graph->vertex(node_index);
	graph->unpack_xz		(best_node,x1,z1);
	if ((x1 != x3) || (z1 != z3))
		return				(false);
	
	m_params->m_distance	= graph->distance(node_index,m_params->m_position);
	m_params->m_vertex_id	= node_index;
	return					(true);
}

TEMPLATE_SPECIALIZATION
template <typename T>
IC	void CLevelPositionPathManager::create_path		(T &vertex)
{
	if (path)
		inherited::create_path(vertex);
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelPositionPathManager