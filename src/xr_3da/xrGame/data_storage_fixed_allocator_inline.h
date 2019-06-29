////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_fixed_allocator_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 27.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage fixed allocator inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<u32 reserved_vertex_count>\
	template<typename _vertex>

#define CDataStorageAllocator	CFixedAllocator<reserved_vertex_count>::CDataStorage<_vertex>

TEMPLATE_SPECIALIZATION
IC	CDataStorageAllocator::CDataStorage					()
{
	u32						memory_usage = 0;
	u32						byte_count;

	byte_count				= (reserved_vertex_count)*sizeof(CGraphVertex);
	m_vertices				= (CGraphVertex*)xr_malloc(byte_count);
	ZeroMemory				(m_vertices,byte_count);
	memory_usage			+= byte_count;
}

TEMPLATE_SPECIALIZATION
CDataStorageAllocator::~CDataStorage					()
{
	xr_free					(m_vertices);
}

TEMPLATE_SPECIALIZATION
IC	void CDataStorageAllocator::init					()
{
	m_vertex_count			= 0;
}

TEMPLATE_SPECIALIZATION
IC	u32  CDataStorageAllocator::get_visited_node_count	() const
{
	return					(m_vertex_count);
}

TEMPLATE_SPECIALIZATION
IC	typename CDataStorageAllocator::CGraphVertex &CDataStorageAllocator::create_vertex		()
{
	VERIFY					(m_vertex_count < reserved_vertex_count - 1);
	return					(*(m_vertices + m_vertex_count++));
}

#undef TEMPLATE_SPECIALIZATION
#undef CDataStorageAllocator