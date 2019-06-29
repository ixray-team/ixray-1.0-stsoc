////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_table_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Table path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _cell_type,\
		u32		 rows,\
		u32		 columns,\
		typename _DataStorage,\
		typename _Parameters,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CTablePathManager CPathManager<CTestTable<_cell_type,rows,columns>,_DataStorage,_Parameters,_dist_type,_index_type,_iteration_type>

TEMPLATE_SPECIALIZATION
CTablePathManager::~CPathManager			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CTablePathManager::begin					(const _index_type &vertex_id, const_iterator &begin, const_iterator &end)
{
	m_best_node_index		= vertex_id;
	inherited::begin		(m_best_node_index,begin,end);
}

TEMPLATE_SPECIALIZATION
IC	void CTablePathManager::setup			(
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
	_i						= goal_node_index / (columns + 2);
	_j						= goal_node_index % (columns + 2);
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CTablePathManager::evaluate	(const _index_type &node_index1, const _index_type &node_index2, const typename _Graph::const_iterator &i) const
{
	VERIFY					(graph);
	return					(graph->get_edge_weight(node_index1,node_index2));
}

TEMPLATE_SPECIALIZATION
IC	_dist_type CTablePathManager::estimate	(const _index_type &node_index) const
{
	VERIFY					(graph);
	// Manhattan heuritics
	int						i,j;
	i						= node_index / (columns + 2);
	j						= node_index % (columns + 2);
	return					(_abs(_j - j) + _abs(_i - i));
}

TEMPLATE_SPECIALIZATION
IC	bool CTablePathManager::is_limit_reached(const _iteration_type	iteration_count) const
{
	VERIFY					(data_storage);
	return					(false);
}

#undef TEMPLATE_SPECIALIZATION
#undef CTablePathManager