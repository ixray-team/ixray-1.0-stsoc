////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_position.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level position path manager
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
		SPosition<
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
	typedef SPosition<
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
	_Parameters			*m_params;

public:
	virtual				~CPathManager	();
	IC		void		setup			(const _Graph *graph, _DataStorage *_data_storage, xr_vector<_index_type> *_path, const _index_type	&_start_node_index, const _index_type &_goal_node_index, _Parameters &params);
	IC		void		init			();
	IC		bool		is_goal_reached	(const _index_type &node_index);
	template <typename T>
	IC		void		create_path		(T &vertex);
};

#include "path_manager_level_position_inline.h"