////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_allocator_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Data storage allocator inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<typename _vertex>

#define CAllocator	CDataStorageAllocator::CDataStorage<_vertex>

TEMPLATE_SPECIALIZATION
IC	CAllocator::CDataStorage				()
{
}

TEMPLATE_SPECIALIZATION
CAllocator::~CDataStorage					()
{
	xr_vector<CGraphVertex*>::iterator	I = m_nodes.begin();
	xr_vector<CGraphVertex*>::iterator	E = m_nodes.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
}

TEMPLATE_SPECIALIZATION
IC	void CAllocator::init					()
{
	xr_vector<CGraphVertex*>::iterator	I = m_nodes.begin();
	xr_vector<CGraphVertex*>::iterator	E = m_nodes.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
	m_nodes.clear			();
}

TEMPLATE_SPECIALIZATION
IC	u32  CAllocator::get_visited_node_count	() const
{
	return					(m_nodes.size());
}

TEMPLATE_SPECIALIZATION
IC	typename CAllocator::CGraphVertex &CAllocator::create_vertex()
{
	m_nodes.push_back		(xr_new<CGraphVertex>());
	return					(*m_nodes.back());
}

#undef TEMPLATE_SPECIALIZATION
#undef CAllocator