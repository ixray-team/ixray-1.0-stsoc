////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_binary_heap_list_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Binary m_heap list data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<u32 heap_count>\
	template <\
		typename _data_storage,\
		template <typename _T> class _vertex\
	>

#define CBinaryHeapList			CDataStorageBinaryHeapList<heap_count>::CDataStorage<_data_storage,_vertex>

TEMPLATE_SPECIALIZATION
IC	CBinaryHeapList::CDataStorage			(const u32 vertex_count, const _dist_type _max_distance) :
		inherited(vertex_count)
{
	u32						memory_usage = 0;
	u32						byte_count;

	u32						temp = vertex_count/heap_count + 1;
	byte_count				= temp*sizeof(CGraphVertex*);
	for (u32 i=0; i<heap_count; ++i) {
		heaps[i].m_heap		= xr_alloc<CGraphVertex*>(temp);
		ZeroMemory			(heaps[i].m_heap,byte_count);
		memory_usage		+= byte_count;
	}
}

TEMPLATE_SPECIALIZATION
CBinaryHeapList::~CDataStorage				()
{
	for (u32 i=0; i<heap_count; ++i)
		xr_free				(heaps[i].m_heap);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::init						()
{
	inherited::init			();
	for (u32 i=0; i<heap_count; ++i)
		heaps[i].m_heap_head= heaps[i].m_heap_tail = heaps[i].m_heap;
}

TEMPLATE_SPECIALIZATION
IC	u32	 CBinaryHeapList::compute_heap_index		(const _index_type &vertex_id) const
{
	return					(u32(vertex_id) % heap_count);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::push_front				(CGraphVertex &vertex)
{
	for (CGraphVertex *i = vertex.prev(); ;i = i->prev())
		if (i->f() <= vertex.f()) {
			vertex.next()		= i->next();
			vertex.prev()		= i;
			i->next()->prev()	= &vertex;
			i->next()			= &vertex;
			return;
		}
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::push_back					(CGraphVertex &vertex)
{
	for (CGraphVertex *i = vertex.next(); ;i = i->next())
		if (i->f() >= vertex.f()) {
			vertex.next()		= i;
			vertex.prev()		= i->prev();
			i->prev()->next()	= &vertex;
			i->prev()			= &vertex;
			return;
		}
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::push_front				(CGraphVertex *old_head, CGraphVertex *new_head)
{
	old_head->prev()->next() = old_head->next();
	new_head->prev() = old_head->next()->prev() = old_head->prev();
	push_front			(*new_head);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::push_back					(CGraphVertex *old_head, CGraphVertex *new_head)
{
	new_head->next() = old_head->prev()->next() = old_head->next();
	old_head->next()->prev() = old_head->prev();
	push_back			(*new_head);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::push_front_if_needed		(CGraphVertex *old_head, CGraphVertex *new_head)
{
	if (old_head != new_head)
		push_front			(old_head,new_head);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::add_opened				(CGraphVertex &vertex)
{
	inherited_base::add_opened	(vertex);
	SBinaryHeap				&m_heap = heaps[compute_heap_index(vertex.index())];
	VERIFY					(m_heap.m_heap_head <= m_heap.m_heap_tail);
	if (m_heap.m_heap_head == m_heap.m_heap_tail) {
		*m_heap.m_heap_tail		= &vertex;
		++m_heap.m_heap_tail;
		(*m_heap.m_heap_head)->next() = m_list_head->next();
		push_back			(**m_heap.m_heap_head);
		return;
	}
	CGraphVertex				*m_heap_head = *m_heap.m_heap_head;
	*m_heap.m_heap_tail			= &vertex;
	std::push_heap			(m_heap.m_heap_head,++m_heap.m_heap_tail,CGraphNodePredicate());
	push_front_if_needed	(m_heap_head, *m_heap.m_heap_head);
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::decrease_opened			(CGraphVertex &vertex, const _dist_type value)
{
	VERIFY					(!is_opened_empty());
	SBinaryHeap				&m_heap = heaps[compute_heap_index(vertex.index())];
	CGraphVertex				*m_heap_head = *m_heap.m_heap_head;
	for (CGraphVertex **i = m_heap.m_heap_head; *i != &vertex; ++i);
	std::push_heap			(m_heap.m_heap_head,i + 1,CGraphNodePredicate());
	if (&vertex != m_heap_head)
		push_front_if_needed(m_heap_head, *m_heap.m_heap_head);
	else
		if (m_heap_head->prev()->f() > vertex.f()) {
			m_heap_head->next()->prev() = m_heap_head->prev();
			m_heap_head->prev()->next() = m_heap_head->next();
			push_front			(vertex);
		}
}

TEMPLATE_SPECIALIZATION
IC	void CBinaryHeapList::remove_best_opened		()
{
	VERIFY					(!is_opened_empty());
	SBinaryHeap				*best_heap = heaps + compute_heap_index(m_list_head->next()->index());
	CGraphVertex				*m_heap_head = *best_heap->m_heap_head;
	std::pop_heap			(best_heap->m_heap_head,best_heap->m_heap_tail--,CGraphNodePredicate());
	m_heap_head->next()->prev() = m_heap_head->prev();
	m_heap_head->prev()->next() = m_heap_head->next();
	if (best_heap->m_heap_head != best_heap->m_heap_tail) {
		(*best_heap->m_heap_head)->next() = m_list_head->next();
		push_back			(**best_heap->m_heap_head);
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CBinaryHeapList