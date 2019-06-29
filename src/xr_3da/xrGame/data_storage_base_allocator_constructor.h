////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_base_allocator_constructor.h
//	Created 	: 21.03.2002
//  Modified 	: 28.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage base allocator constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _base,
	typename _allocator
>
struct CConstructorBaseAllocator {
	template <template <typename _T> class _vertex> 
	class CDataStorage : 
		public _base::CDataStorage<_vertex>,
		public _allocator::CDataStorage<typename _base::CDataStorage<_vertex>::CGraphVertex>
	{
	public:
		typedef typename _base::CDataStorage<_vertex>	CDataStorageBase;
		typedef typename _allocator::CDataStorage<
			typename _base::CDataStorage<
				_vertex
			>::CGraphVertex
		>												CDataStorageAllocator;
		typedef typename CDataStorageBase::CGraphVertex CGraphVertex;
		typedef typename CGraphVertex::_index_type		_index_type;

	public:
		IC							CDataStorage	(const u32 vertex_count);
		virtual						~CDataStorage	();
		IC		void				init			();
	};
};

#include "data_storage_base_allocator_constructor_inline.h"