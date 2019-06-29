////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_index_base_allocator_constructor.h
//	Created 	: 21.03.2002
//  Modified 	: 28.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage index base allocator constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "data_storage_base_allocator_constructor.h"

template <
	typename _index, 
	typename _base,
	typename _allocator,
	template <
		typename _base,
		typename _allocator
	>
	class	 _base_allocator_constructor = CConstructorBaseAllocator
>
struct CConstructorVertex {
	template <
		template <typename T> class _vertex = CEmptyClassTemplate,
		template <typename T1, typename T2> class _index_vertex = CEmptyClassTemplate2
	>
	class CDataStorage : 
		public _index::CDataStorage<
			_vertex,
			_index_vertex,
			_base_allocator_constructor<
				_base,
				_allocator
			>
		>
	{
	public:
		typedef typename _index::CDataStorage<
			_vertex,
			_index_vertex,
			_base_allocator_constructor<
			_base,
			_allocator
			>
		> inherited;
		typedef typename inherited::inherited		inherited_allocator;
		typedef typename inherited::CGraphVertex	CGraphVertex;
		typedef typename CGraphVertex::_index_type	_index_type;

	public:
		IC							CDataStorage	(const u32 vertex_count);
		virtual						~CDataStorage	();
		IC		void				init			();
		IC		CGraphVertex		&create_vertex	(const _index_type &vertex_id);
	};
};

#include "data_storage_index_base_allocator_constructor_inline.h"