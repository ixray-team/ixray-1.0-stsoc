////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_priority_queue.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Priority queue data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

template <template	<typename _vertex, typename _predicate> class _priority_queue>
struct CPriorityQueue {

	template <template <typename _T> class T1>
	struct PriorityQueue {
		template<typename T2>
		struct _vertex : public T1<T2> {
			struct CPredicate {
				IC			bool	operator()(const T2 *vertex1, const T2 *vertex2) const
				{
					return				(vertex1->f() > vertex2->f());
				};
			};

			typedef _priority_queue<T2*,CPredicate>	_priority_queue;

			typename _priority_queue::pointer		m_pq_node;
		};
	};

	template <
		typename _data_storage,
		template <typename _T> class _vertex = CEmptyClassTemplate,
		template <typename _T1, typename _T2> class index_vertex = CEmptyClassTemplate2
	>
	class CDataStorage : public _data_storage::CDataStorage<PriorityQueue<_vertex>::_vertex> 
	{
	public:
		typedef typename _data_storage::CDataStorage<PriorityQueue<_vertex>::_vertex> inherited;
		typedef typename inherited::CGraphVertex		CGraphVertex;
		typedef typename inherited::CGraphIndexVertex	CGraphIndexVertex;
		typedef typename CGraphVertex::_dist_type		_dist_type;
		typedef typename CGraphVertex::_index_type		_index_type;
		typedef typename PriorityQueue<_vertex>::_vertex<CGraphVertex>::_priority_queue	_priority_queue;

	protected:
		_priority_queue			m_priority_queue;

	public:
		IC						CDataStorage		(const u32 vertex_count);
		virtual					~CDataStorage		();
		IC		void			init				();
		IC		bool			is_opened_empty		() const;
		IC		void			add_opened			(CGraphVertex &vertex);
		IC		void			decrease_opened		(CGraphVertex &vertex, const _dist_type value);
		IC		void			remove_best_opened	();
		IC		void			add_best_closed		();
		IC		CGraphVertex	&get_best			() const;
	};
};

#include "data_storage_priority_queue_inline.h"