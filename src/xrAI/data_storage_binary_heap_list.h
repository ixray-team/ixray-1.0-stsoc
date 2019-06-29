////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_binary_heap_list.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Binary heap list data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

template <u32 heap_count>
struct CDataStorageBinaryHeapList {

	template <
		typename _data_storage,
		template <typename _T> class _vertex = CEmptyClassTemplate
	>
	class CDataStorage : public CDataStorageDoubleLinkedList<true>::CDataStorage<_data_storage,_vertex> {
	public:
		typedef typename CDataStorageDoubleLinkedList<true>::CDataStorage<
			_data_storage,
			_vertex
		>											inherited;
		typedef typename inherited::inherited_base	inherited_base;
		typedef typename inherited::CGraphVertex	CGraphVertex;
		typedef typename CGraphVertex::_dist_type	_dist_type;
		typedef typename CGraphVertex::_index_type	_index_type;

		struct CGraphNodePredicate {
			IC			bool	operator()(CGraphVertex *node1, CGraphVertex *node2)
			{
				return				(node1->f() > node2->f());
			};
		};

		struct SBinaryHeap {
			CGraphVertex		**m_heap;
			CGraphVertex		**m_heap_head;
			CGraphVertex		**m_heap_tail;
		};

	protected:
		SBinaryHeap				heaps[heap_count];

	public:
		IC						CDataStorage		(const u32 vertex_count, const _dist_type _max_distance = _dist_type(u32(-1)));
		virtual					~CDataStorage		();
		IC		void			init				();
		IC		u32				compute_heap_index	(const _index_type &vertex_id) const;
		IC		void			push_front			(CGraphVertex &vertex);
		IC		void			push_back			(CGraphVertex &vertex);
		IC		void			push_front			(CGraphVertex *old_head, CGraphVertex *new_head);
		IC		void			push_back			(CGraphVertex *old_head, CGraphVertex *new_head);
		IC		void			push_front_if_needed(CGraphVertex *old_head, CGraphVertex *new_head);
		IC		void			add_opened			(CGraphVertex &vertex);
		IC		void			decrease_opened		(CGraphVertex &vertex, const _dist_type value);
		IC		void			remove_best_opened	();
	};
};

#include "data_storage_binary_heap_list_inline.h"