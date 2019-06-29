////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_allocator_generic.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Generic vertex allocator
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CVertexAllocatorGeneric {
	template <typename _vertex>
	class CDataStorage {
	public:
		typedef _vertex								CGraphVertex;
		typedef typename CGraphVertex::_index_type	_index_type;

	protected:
		xr_vector<CGraphVertex*>	m_nodes;

	public:
		IC							CDataStorage			();
		virtual						~CDataStorage			();
		IC		void				init					();
		IC		u32					get_visited_node_count	() const;
		IC		CGraphVertex		&create_vertex			();
	};
};

#include "vertex_allocator_generic_inline.h"