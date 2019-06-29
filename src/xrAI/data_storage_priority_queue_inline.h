////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_priority_queue_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Priority queue data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <template <typename _vertex_, typename _predicate> class _priority_queue>\
	template <\
		typename _data_storage,\
		template <typename _T> class _vertex,\
		template <typename _T1, typename _T2> class _index_vertex\
	>

#define CPriorityQueue	CPriorityQueue<_priority_queue>::CDataStorage<_data_storage,_vertex,_index_vertex>

TEMPLATE_SPECIALIZATION
IC	CPriorityQueue::CDataStorage				(const u32 vertex_count) :
		inherited			(vertex_count)
{
}

TEMPLATE_SPECIALIZATION
CPriorityQueue::~CDataStorage					()
{
}

TEMPLATE_SPECIALIZATION
IC	void CPriorityQueue::init					()
{
	inherited::init			();
	while (!m_priority_queue.empty())
		m_priority_queue.pop();
}

TEMPLATE_SPECIALIZATION
IC	bool CPriorityQueue::is_opened_empty		() const
{
	return					(m_priority_queue.empty());
}

TEMPLATE_SPECIALIZATION
IC	void CPriorityQueue::add_opened				(CGraphVertex &vertex)
{
	inherited::add_opened	(vertex);
	vertex.m_pq_node		= m_priority_queue.push(&vertex);
}

TEMPLATE_SPECIALIZATION
IC	void CPriorityQueue::decrease_opened		(CGraphVertex &vertex, const _dist_type value)
{
	VERIFY					(!is_opened_empty());
	VERIFY					(is_opened(vertex));
	m_priority_queue.increase(vertex.m_pq_node,&vertex);
}

TEMPLATE_SPECIALIZATION
IC	void CPriorityQueue::remove_best_opened		()
{
	VERIFY					(!is_opened_empty());
	m_priority_queue.pop		();
}

TEMPLATE_SPECIALIZATION
IC	void CPriorityQueue::add_best_closed		()
{
	VERIFY					(!is_opened_empty());
	inherited::add_closed	(*m_priority_queue.top());
}

TEMPLATE_SPECIALIZATION
IC	typename CPriorityQueue::CGraphVertex &CPriorityQueue::get_best		() const
{
	VERIFY					(!is_opened_empty());
	return					(*m_priority_queue.top());
}

#undef TEMPLATE_SPECIALIZATION
#undef CPriorityQueue