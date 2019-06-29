////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_base.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Base path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Graph,
	typename _DataStorage,
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
> 	class CPathManagerBase 
{
protected:
	const _Graph			*graph;
	_DataStorage			*data_storage; 
	xr_vector<_index_type>	*path;
	_index_type				start_node_index;
	_index_type				goal_node_index;
	_dist_type				max_range;
	_iteration_type			max_iteration_count;
	_index_type				max_visited_node_count;
	_index_type				best_node_index;
public:
	typedef	typename _Graph::const_iterator const_iterator;

						CPathManagerBase()
	{
		graph			= 0;
		data_storage	= 0;
		path			= 0;
	}

	virtual				~CPathManagerBase()
	{
	}

	IC		void		init			()
	{
	}

	IC		void		setup			(
				const _Graph			*_graph,
				_DataStorage			*_data_storage,
				xr_vector<_index_type>	*_path,
				const _index_type		_start_node_index,
				const _index_type		_goal_node_index,
				const _Parameters		&params
			)
	{
		graph					= _graph;
		data_storage			= _data_storage;
		path					= _path;
		start_node_index		= _start_node_index;
		goal_node_index			= _goal_node_index;
		max_visited_node_count	= params.max_visited_node_count;
		max_range				= params.max_range;
		max_iteration_count		= params.max_iteration_count;
	}

	IC		_dist_type	evaluate		(const _index_type node_index1, const _index_type node_index2, const typename _Graph::const_iterator &i) const
	{
		VERIFY					(graph);
		return					(graph->get_edge_weight(node_index1,node_index2,i));
	}

	IC		_dist_type	estimate		(const _index_type vertex_id) const
	{
		VERIFY					(graph);
		return					(_dist_type(0));
	}

	IC		void		create_path		()
	{
		VERIFY					(data_storage);
//		Msg						("Path [IC=xxx][VNC=%d][BV=%f]",data_storage->get_visited_node_count(),data_storage->get_best().f());
		if (path)
			data_storage->get_path	(*path);
	}

	IC		_index_type	start_node		() const
	{
		return					(start_node_index);
	}

	IC		bool		is_goal_reached	(const _index_type vertex_id) const
	{
		return					(vertex_id == goal_node_index);
	}

	IC		bool		is_limit_reached(const _iteration_type	iteration_count) const
	{
		VERIFY					(data_storage);
		return					(
			(data_storage->get_best().f() >= max_range)	||
			(iteration_count >= max_iteration_count)	||
			(data_storage->get_visited_node_count() >= max_visited_node_count)
		);
	}

	IC		bool		is_accessible	(const _index_type vertex_id) const
	{
		VERIFY					(graph);
		return					(graph->is_accessible(vertex_id));
	}

	IC		bool		is_metric_euclidian() const
	{
		// #pragma todo("Dima to Dima : implement path manager for non-euclidian metrics")
		return					(true);
	}

	IC		void		begin			(const _index_type vertex_id, const_iterator &begin, const_iterator &end)
	{
		best_node_index			= vertex_id;
		graph->begin			(vertex_id,begin,end);
	}

	IC		u32			get_value		(const_iterator &i) const
	{
		return					(graph->value(best_node_index,i));
	}

	IC		void		finalize		()
	{
	}
};

