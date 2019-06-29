////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_cheap_list_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Cheap list data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		u32			cheap_node_count,\
		bool		keep_fixed_count,\
		bool		m_keep_filled\
	>\
	template <\
		typename _data_storage,\
		template <typename _T> class _vertex\
	>

#define CCheapList CDataStorageCheapList<cheap_node_count,keep_fixed_count,m_keep_filled>::CDataStorage<_data_storage,_vertex>

TEMPLATE_SPECIALIZATION
IC	CCheapList::CDataStorage			(const u32 vertex_count, const _dist_type _max_distance) :
	inherited				(vertex_count)
{
}

TEMPLATE_SPECIALIZATION
CCheapList::~CDataStorage				()
{
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::init				()
{
	inherited::init			();
	m_cheap_list_head		= m_cheap_list_tail = 0;
	m_cheap_count			= 0;
	if (m_keep_filled)
		m_filled_over		= false;
}

TEMPLATE_SPECIALIZATION
IC	bool CCheapList::is_opened_empty	() const
{
	return					(inherited::is_opened_empty() && (!m_cheap_list_head));
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::verify_cheap		() const
{
//		u32						count = 0;
//		for (CGraphVertex *i = m_cheap_list_head; i; i = i->next(), ++count) {
//			VERIFY(!i->next() || (i->f() <= i->next()->f()));
//		}
//		VERIFY					(count == m_cheap_count);
//		count					= 0;
//		for (CGraphVertex *i = m_cheap_list_tail; i; i = i->prev(), ++count) {
//			VERIFY(!i->prev() || (i->f() >= i->prev()->f()));
//		}
//		VERIFY					(count == m_cheap_count);
//		count					= 0;
//		for (CGraphVertex *i = m_list_head->next(); i != m_list_tail; i = i->next(), ++count) {
//			VERIFY				(i->f() >= m_cheap_list_tail->f());
//		}
//		u32						count1 = 0;
//		for (CGraphVertex *i = m_list_tail->prev(); i != m_list_head; i = i->prev(), ++count1) {
//			VERIFY				(i->f() >= m_cheap_list_tail->f());
//		}
//		VERIFY					(count == count1);
//		VERIFY					((m_cheap_count < cheap_node_count) || m_filled_over);
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::add_opened			(CGraphVertex &vertex)
{
	verify_cheap				();
	if (!add_cheap_count(vertex)) {
		verify_cheap			();
		inherited::add_opened	(vertex);
		verify_cheap			();
	}
	else {
		inherited_base::add_opened	(vertex);
		verify_cheap			();
	}
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::add_cheap			(CGraphVertex &vertex)
{
	if (m_cheap_list_head->f() >= vertex.f()) {
		m_cheap_list_head->prev()	= &vertex;
		vertex.next()				= m_cheap_list_head;
		vertex.prev()				= 0;
		m_cheap_list_head			= &vertex;
		++m_cheap_count;
		return;
	}

//		if ((m_cheap_list_tail->f() - vertex.f())*1 < (vertex.f() - m_cheap_list_head->f())*3)
		for (CGraphVertex *i = m_cheap_list_tail->prev(); ; i = i->prev()) {
			if (i->f() <= vertex.f()) {
				vertex.next()		= i->next();
				vertex.prev()		= i;
				i->next()->prev()	= &vertex;
				i->next()			= &vertex;
				++m_cheap_count;
				return;
			}
		}
//		else
//			for (CGraphVertex *i = m_cheap_list_head->next(); ; i = i->next()) {
//				if (i->f() >= vertex.f()) {
//					vertex.next()		= i;
//					vertex.prev()		= i->prev();
//					i->prev()->next()	= &vertex;
//					i->prev()			= &vertex;
//					++m_cheap_count;
//					return;
//				}
//			}
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::add_cheap_tail		(CGraphVertex &vertex)
{
	if (m_cheap_list_tail->f() < vertex.f()) {
		m_cheap_list_tail->next() = &vertex;
		vertex.prev()		= m_cheap_list_tail;
		vertex.next()		= 0;
		m_cheap_list_tail	= &vertex;
		++m_cheap_count;
		return;
	}
	add_cheap			(vertex);
}

TEMPLATE_SPECIALIZATION
IC	bool CCheapList::add_cheap_count	(CGraphVertex &vertex)
{
	if (!m_cheap_list_tail) {
		m_cheap_list_head		= m_cheap_list_tail = &vertex;
		vertex.next()			= vertex.prev() = 0;
		m_cheap_count			= 1;
		verify_cheap		();
		return				(true);
	}

	if (m_keep_filled && !m_filled_over) {
		verify_cheap		();
		add_cheap_tail		(vertex);
		if (m_cheap_count == cheap_node_count)
			m_filled_over		= true;
		verify_cheap		();
		return				(true);
	}

	if (m_cheap_list_tail->f() < vertex.f()) {
		verify_cheap		();
		return				(false);
	}

	add_cheap				(vertex);

	if (keep_fixed_count && (m_cheap_count > cheap_node_count))
		remove_cheap_tail	();

	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::remove_cheap_tail	()
{
	m_cheap_list_tail->prev()->next() = 0;
	m_cheap_list_tail->next()		= m_list_head->next();
	m_list_head->next()->prev()		= m_cheap_list_tail;
	m_list_head->next()				= m_cheap_list_tail;
	m_cheap_list_tail				= m_cheap_list_tail->prev();
	m_list_head->next()->prev()		= m_list_head;
	--m_cheap_count;
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::fill_cheap			()
{
	if (m_keep_filled)
		m_filled_over			= false;
	if (m_list_head->next() == m_list_tail) {
		m_cheap_list_head		= m_cheap_list_tail = 0;
		m_cheap_count			= 0;
		return;
	}
	
	m_cheap_list_head			= m_cheap_list_tail = m_list_head->next();
	m_list_head->next()			= m_list_head->next()->next();
	m_list_head->next()->prev()	= m_list_head;
	m_cheap_list_head->prev()	= m_cheap_list_head->next() = 0;

	m_cheap_count				= 1;
	for (CGraphVertex *i = m_list_head->next(), *j; i != m_list_tail; i = i->next()) {
		if (m_cheap_count < cheap_node_count) {
			j				= i->prev();
			i->prev()->next()	= i->next();
			i->next()->prev()	= i->prev();
			add_cheap_tail	(*i);
			i				= j;
			continue;
		}
		else {
			if (m_keep_filled)
				m_filled_over	= true;
			for ( ; i != m_list_tail; i = i->next()) {
				if (i->f() < m_cheap_list_tail->f()) {
					j							= i->prev();
					i->prev()->next()				= i->next();
					i->next()->prev()				= i->prev();
					add_cheap					(*i);
					i							= j;
					remove_cheap_tail			();
				}
			}
			return;
		}
	}
	VERIFY			(m_cheap_count <= cheap_node_count);
	if (m_keep_filled && (m_cheap_count == cheap_node_count))
		m_filled_over	= true;
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::decrease_opened	(CGraphVertex &vertex, const _dist_type value)
{
	VERIFY					(!is_opened_empty());
	VERIFY					(m_cheap_list_head && m_cheap_list_tail);
	
//		verify_cheap			();
	for (CGraphVertex *i = m_cheap_list_head; i && (i != &vertex); i = i->next());
	if (i == &vertex) {
		if (i->prev() && (i->prev()->f() > vertex.f())) {
			for ( ; i->prev() && (i->prev()->f() > vertex.f()); i = i->prev());
			if (!i->prev()) {
				vertex.prev()->next() = vertex.next();
				if (vertex.next())
					vertex.next()->prev() = vertex.prev();
				else
					m_cheap_list_tail = vertex.prev();
				vertex.prev()		= 0;
				vertex.next()		= m_cheap_list_head;
				m_cheap_list_head->prev() = &vertex;
				m_cheap_list_head	= &vertex;
				verify_cheap			();
				return;
			}
			else {
				if (vertex.prev())
					vertex.prev()->next() = vertex.next();
				else
					m_cheap_list_tail = vertex.prev();
				if (vertex.next())
					vertex.next()->prev() = vertex.prev();
				else
					m_cheap_list_tail = vertex.prev();
				vertex.next()		= i;
				vertex.prev()		= i->prev();
				i->prev()->next()	= &vertex;
				i->prev()			= &vertex;
				verify_cheap			();
				return;
			}
		}
	}
	else {
		if (keep_fixed_count) {
			vertex.next()->prev()			= vertex.prev();
			vertex.prev()->next()			= vertex.next();
			if (!add_cheap_count(vertex)) {
				vertex.next()->prev()		= &vertex;
				vertex.prev()->next()		= &vertex;
			}
		}
		else {
			CGraphVertex				*next = vertex.next(), *prev = vertex.prev();
			if (add_cheap_count(vertex)) {
				next->prev()			= prev;
				prev->next()			= next;
			}
		}
		verify_cheap			();
	}
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::remove_best_opened	()
{
	VERIFY					(!is_opened_empty());
	VERIFY					(m_cheap_list_head);
	
	verify_cheap			();
	m_cheap_list_head			= m_cheap_list_head->next();
	--m_cheap_count;
	
	if (m_cheap_list_head)
		m_cheap_list_head->prev() = 0;
	else
		fill_cheap			();
	verify_cheap			();
}

TEMPLATE_SPECIALIZATION
IC	void CCheapList::add_best_closed	()
{
	VERIFY						(!is_opened_empty());
	VERIFY						(m_cheap_list_head);
	inherited_base::add_closed	(*m_cheap_list_head);
}

TEMPLATE_SPECIALIZATION
IC	typename CCheapList::CGraphVertex &CCheapList::get_best	() const
{
	VERIFY					(!is_opened_empty());
	VERIFY					(m_cheap_list_head && m_cheap_list_tail);
	return					(*m_cheap_list_head);
}

#undef TEMPLATE_SPECIALIZATION
#undef CCheapList