////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_multi_binary_heap_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Multi binary m_heaps data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<u32 heap_count>\
	template<\
		typename _data_storage,\
		template <typename _T> class _vertex\
	>

#define CMultiBinaryHeap		CDataStorageMultiBinaryHeap<heap_count>::CDataStorage<_data_storage,_vertex>

TEMPLATE_SPECIALIZATION
IC	CMultiBinaryHeap::CDataStorage			(const u32 vertex_count) : 
		inherited			(vertex_count)
{
	u32						memory_usage = 0;
	u32						byte_count;

	u32						temp = vertex_count/heap_count + 1;
	byte_count				= temp*sizeof(CGraphVertex*);
	for (u32 i=0; i<heap_count; ++i) {
		m_heaps[i].m_heap	= xr_alloc<CGraphVertex*>(temp);
		ZeroMemory			(m_heaps[i].m_heap,byte_count);
		memory_usage		+= byte_count;
	}
}

TEMPLATE_SPECIALIZATION
CMultiBinaryHeap::~CDataStorage				()
{
	for (u32 i=0; i<heap_count; ++i)
		xr_free				(m_heaps[i].m_heap);
}

TEMPLATE_SPECIALIZATION
IC	void CMultiBinaryHeap::init						()
{
	inherited::init			();
	for (u32 i=0; i<heap_count; ++i)
		m_heaps[i].m_heap_head	= m_heaps[i].m_heap_tail = m_heaps[i].m_heap;
}

TEMPLATE_SPECIALIZATION
IC	bool CMultiBinaryHeap::is_opened_empty			() const
{
	for (u32 i=0; i<heap_count; ++i) {
		VERIFY				(m_heaps[i].m_heap_head <= m_heaps[i].m_heap_tail);
		if (m_heaps[i].m_heap_head != m_heaps[i].m_heap_tail)
			return			(false);
	}
	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	void CMultiBinaryHeap::add_opened				(CGraphVertex &vertex)
{
	inherited::add_opened	(vertex);
	SBinaryHeap				&m_heap = m_heaps[vertex.index() % heap_count];
	VERIFY					(m_heap.m_heap_head <= m_heap.m_heap_tail);
	*m_heap.m_heap_tail		= &vertex;
	std::push_heap			(m_heap.m_heap_head,++m_heap.m_heap_tail,CGraphNodePredicate());
}

TEMPLATE_SPECIALIZATION
IC	void CMultiBinaryHeap::decrease_opened			(CGraphVertex &vertex, const _dist_type value)
{
	VERIFY					(!is_opened_empty());
	SBinaryHeap				&m_heap = m_heaps[vertex.index() % heap_count];
	for (CGraphVertex **i = m_heap.m_heap_head; *i != &vertex; ++i);
	std::push_heap			(m_heap.m_heap_head,i + 1,CGraphNodePredicate());
}

TEMPLATE_SPECIALIZATION
IC	void CMultiBinaryHeap::remove_best_opened		()
{
	VERIFY					(!is_opened_empty());
	std::pop_heap			(m_best_heap->m_heap_head,m_best_heap->m_heap_tail--,CGraphNodePredicate());
}

TEMPLATE_SPECIALIZATION
IC	void CMultiBinaryHeap::add_best_closed			()
{
	VERIFY					(!is_opened_empty());
	inherited::add_closed	(**m_best_heap->m_heap_head);
}

TEMPLATE_SPECIALIZATION
IC	typename CMultiBinaryHeap::CGraphVertex &CMultiBinaryHeap::get_best			()
{
	VERIFY					(!is_opened_empty());
	m_best_heap				= 0;
	_dist_type				f = _dist_type(0x7fffffff);
	for (u32 i=0; i<heap_count; ++i) {
		if ((m_heaps[i].m_heap_head < m_heaps[i].m_heap_tail) && (*m_heaps[i].m_heap_head)->f() < f) {
			m_best_heap		= m_heaps + i;
			f				= (*m_heaps[i].m_heap_head)->f();
		}
	}
	VERIFY					(m_best_heap);
	return					(**m_best_heap->m_heap_head);
}

#undef TEMPLATE_SPECIALIZATION
#undef CMultiBinaryHeap