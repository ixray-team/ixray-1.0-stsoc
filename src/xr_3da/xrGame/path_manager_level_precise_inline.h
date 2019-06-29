////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_precise_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 12.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Level path manager precise inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelPathManager CLevelPathManagerPrecise<\
	_DataStorage,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>

TEMPLATE_SPECIALIZATION
CLevelPathManager::~CLevelPathManagerPrecise()
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelPathManager::setup			(
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
}

TEMPLATE_SPECIALIZATION
IC	void CLevelPathManager::init			()
{
	VERIFY					(graph);

	m_distance_xz			= graph->header().cell_size();
	m_sqr_distance_xz		= _sqr(graph->header().cell_size());
	m_square_size_y			= _sqr((float)(graph->header().factor_y()/32767.0));
	
	const _Graph::CVertex	&tNode1	= *graph->vertex(start_node_index);
	graph->unpack_xz		(tNode1,x1,z1);
	y1						= tNode1.position().y();
	
	const _Graph::CVertex	&tNode2	= *graph->vertex(goal_node_index);
	graph->unpack_xz		(tNode2,x3,z3);
	y3						= tNode2.position().y();
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelPathManager::evaluate	(const _index_type &node_index1, const _index_type &node_index2, const _Graph::const_iterator &/**i/**/)
{
	return					(1.f);//m_distance_xz);
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CLevelPathManager::estimate	(const _index_type &node_index) const
{
	return					(_sqrt((float)(m_sqr_distance_xz*float(_sqr(x3 - x1) + _sqr(z3 - z1))/**/ + m_square_size_y*(float)_sqr(y3 - y1)/**/)));
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelPathManager::is_goal_reached	(const _index_type &node_index)
{
	if (node_index == goal_node_index)
		return				(true);
	
	best_node				= graph->vertex(node_index);
	graph->unpack_xz		(best_node,x1,z1);
	y1						= best_node->position().y();

	return					(false);
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelPathManager::is_limit_reached(const _iteration_type	iteration_count) const
{
	VERIFY					(data_storage);
	return					(inherited::is_limit_reached(iteration_count));
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelPathManager::is_accessible	(const _index_type &vertex_id) const
{
	return					(graph->valid_vertex_id(vertex_id));
}

TEMPLATE_SPECIALIZATION
IC	void CLevelPathManager::begin			(const _index_type &vertex_id, const_iterator &begin, const_iterator &end)
{
	graph->begin			(best_node,begin,end);
}

TEMPLATE_SPECIALIZATION
IC	const _index_type CLevelPathManager::get_value		(const_iterator &i) const
{
	return					(graph->value(best_node,i));
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelPathManager