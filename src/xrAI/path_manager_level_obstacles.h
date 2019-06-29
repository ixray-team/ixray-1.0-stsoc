////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_obstacles.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level obstacles path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_level.h"

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelNavigationGraph,
		_DataStorage,
		SObstacleParams<
			_dist_type,
			_index_type,
			_iteration_type
		>,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManager <
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
	typedef CLevelNavigationGraph _Graph;
	typedef SObstacleParams<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				SBaseParameters<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;
protected:
	bool				m_avoid_dynamic_obstacles;

public:
	virtual				~CPathManager	();
	IC		void		setup			(const _Graph *graph, _DataStorage *_data_storage, xr_vector<_index_type> *_path, const _index_type	&_start_node_index, const _index_type &_goal_node_index, const _Parameters &params);
	IC		bool		is_accessible	(const _index_type &vertex_id) const;
};

#include "path_manager_level_obstacles_inline.h"