////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_precise.h
//	Created 	: 21.03.2002
//  Modified 	: 12.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Level path manager precise
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_navigation_graph.h"
#include "path_manager_generic.h"

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CLevelPathManagerPrecise : public CPathManagerGeneric <
			CLevelNavigationGraph,
			_DataStorage,
			SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
protected:
	typedef CLevelNavigationGraph _Graph;
	typedef SBaseParameters<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManagerGeneric <
				_Graph,
				_DataStorage,
				_Parameters,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;

protected:
	_Graph::CVertex		*best_node;
	float				m_distance_xz;
	float				m_sqr_distance_xz;
	float				m_square_size_y;
	int					x1;
	int					y1;
	int					z1;
	int					x3;
	int					y3;
	int					z3;

public:
	virtual				~CLevelPathManagerPrecise	();
	IC		void		setup						(const _Graph *graph, _DataStorage *_data_storage, xr_vector<_index_type> *_path, const _index_type	&_start_node_index, const _index_type &_goal_node_index, const _Parameters &params);
	IC		void		init						();
	IC		_dist_type	evaluate					(const _index_type &node_index1, const _index_type &node_index2, const _Graph::const_iterator &i);
	IC		_dist_type	estimate					(const _index_type &node_index) const;
	IC		bool		is_goal_reached				(const _index_type &node_index);
	IC		bool		is_limit_reached			(const _iteration_type iteration_count) const;
	IC		bool		is_accessible				(const _index_type &vertex_id) const;
	IC		void		begin						(const _index_type &vertex_id, const_iterator &begin, const_iterator &end);
	IC		const _index_type get_value				(const_iterator &i) const;
};

#include "path_manager_level_precise_inline.h"