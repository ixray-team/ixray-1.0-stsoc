////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_game_selector.h
//	Created 	: 21.03.2002
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_params.h"

namespace PathManagers {
	template <
		typename _dist_type,
		typename _index_type,
		typename _iteration_type
	>
	struct SVertexType : public SBaseParameters<
		_dist_type,
		_index_type,
		_iteration_type
	> {
		u8				m_vertex_types[LOCATION_TYPE_COUNT];
		_index_type		m_vertex_id;
		xr_vector<_index_type> *m_path;

		IC	SVertexType (
				const u8				*vertex_types,
				_dist_type				max_range = _dist_type(6000),
				_iteration_type			max_iteration_count = _iteration_type(-1),
				_index_type				max_visited_node_count = _index_type(-1)
			)
			:
			SBaseParameters<
				_dist_type,
				_index_type,
				_iteration_type
			>(
				max_range,
				max_iteration_count,
				max_visited_node_count
			),
			m_vertex_types(vertex_types)
		{
		}

		IC	_index_type	selected_vertex_id() const
		{
			return		(m_vertex_id);
		}
	};
};