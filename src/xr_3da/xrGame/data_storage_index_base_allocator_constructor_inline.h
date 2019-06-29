////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_index_base_allocator_constructor_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 28.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage index base allocator constructor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<\
		typename _index,\
		typename _base,\
		typename _allocator,\
		template <\
			typename _base,\
			typename _allocator\
		>\
		class	 _base_allocator_constructor\
	>\
	template <\
		template <typename T> class _vertex,\
		template <typename T1, typename T2> class _index_vertex\
	>

#define CDataStorageIndexBaseAllocatorConstructor \
	CConstructorVertex<\
		_index,\
		_base,\
		_allocator,\
		_base_allocator_constructor\
	>::CDataStorage<_vertex,_index_vertex>

TEMPLATE_SPECIALIZATION
IC	CDataStorageIndexBaseAllocatorConstructor::CDataStorage	(const u32 vertex_count) :
		inherited				(vertex_count)
{
}

TEMPLATE_SPECIALIZATION
CDataStorageIndexBaseAllocatorConstructor::~CDataStorage	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CDataStorageIndexBaseAllocatorConstructor::init	()
{
	inherited::init				();
}

TEMPLATE_SPECIALIZATION
IC	typename CDataStorageIndexBaseAllocatorConstructor::CGraphVertex &CDataStorageIndexBaseAllocatorConstructor::create_vertex	(const _index_type &vertex_id)
{
	return						(inherited::create_vertex(inherited_allocator::create_vertex(),vertex_id));
}

#undef TEMPLATE_SPECIALIZATION
#undef CDataStorageIndexBaseAllocatorConstructor