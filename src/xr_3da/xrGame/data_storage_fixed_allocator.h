////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_fixed_allocator.h
//	Created 	: 21.03.2002
//  Modified 	: 27.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage fixed allocator
////////////////////////////////////////////////////////////////////////////

#pragma once

template <u32 reserved_vertex_count>
struct CFixedAllocator {
	template <typename _vertex>
	class CDataStorage {
	public:
		typedef _vertex								CGraphVertex;
		typedef typename CGraphVertex::_index_type	_index_type;

	protected:
		u32							m_vertex_count;
		CGraphVertex				*m_vertices;

	public:
		IC							CDataStorage			();
		virtual						~CDataStorage			();
		IC		void				init					();
		IC		u32					get_visited_node_count	() const;
		IC		CGraphVertex		&create_vertex			();
	};
};

#include "data_storage_fixed_allocator_inline.h"