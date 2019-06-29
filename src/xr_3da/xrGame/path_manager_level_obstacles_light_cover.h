////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_obstacles_light_cover.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level obstacles light cover path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_level_obstacles.h"

template <
	typename _DataStorage,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CLevelNavigationGraph,
		_DataStorage,
		SObstaclesLightCover<
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
			SObstacleParams<
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
	typedef SObstaclesLightCover<
		_dist_type,
		_index_type,
		_iteration_type
	> _Parameters;
	typedef typename CPathManager <
				_Graph,
				_DataStorage,
				SObstacleParams<
					_dist_type,
					_index_type,
					_iteration_type
				>,
				_dist_type,
				_index_type,
				_iteration_type
			> inherited;

protected:
	float				light_weight;
	float				cover_weight;
	float				distance_weight;

public:
	virtual				~CPathManager	();
	IC		void		setup			(const _Graph *graph, _DataStorage *_data_storage, xr_vector<_index_type> *_path, const _index_type	&_start_node_index, const _index_type &_goal_node_index, const _Parameters &params);
	IC		_dist_type	evaluate		(const _index_type &node_index1, const _index_type &node_index2, const _Graph::const_iterator &i);
	IC		_dist_type	estimate		(const _index_type &node_index) const;
};

#include "path_manager_level_obstacles_light_cover_inline.h"