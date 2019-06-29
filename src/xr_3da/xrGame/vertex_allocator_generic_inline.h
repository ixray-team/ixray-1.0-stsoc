////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_allocator_generic_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Generic vertex allocator inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<typename _vertex>

#define CGenericVertexAllocator	CVertexAllocatorGeneric::CDataStorage<_vertex>

TEMPLATE_SPECIALIZATION
IC	CGenericVertexAllocator::CDataStorage				()
{
}

TEMPLATE_SPECIALIZATION
CGenericVertexAllocator::~CDataStorage					()
{
	xr_vector<CGraphVertex*>::iterator	I = m_nodes.begin();
	xr_vector<CGraphVertex*>::iterator	E = m_nodes.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
}

TEMPLATE_SPECIALIZATION
IC	void CGenericVertexAllocator::init					()
{
	xr_vector<CGraphVertex*>::iterator	I = m_nodes.begin();
	xr_vector<CGraphVertex*>::iterator	E = m_nodes.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
	m_nodes.clear			();
}

TEMPLATE_SPECIALIZATION
IC	u32  CGenericVertexAllocator::get_visited_node_count	() const
{
	return					(m_nodes.size());
}

TEMPLATE_SPECIALIZATION
IC	typename CGenericVertexAllocator::CGraphVertex &CGenericVertexAllocator::create_vertex()
{
	m_nodes.push_back		(xr_new<CGraphVertex>());
	return					(*m_nodes.back());
}

#undef TEMPLATE_SPECIALIZATION
#undef CGenericVertexAllocator