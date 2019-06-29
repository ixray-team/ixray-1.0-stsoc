////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_cheap_list.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Cheap list data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "data_storage_double_linked_list.h"

template <
	u32		cheap_node_count,
	bool	keep_fixed_count,
	bool	keep_filled
>
struct CDataStorageCheapList {

	template <
		typename _data_storage,
		template <typename _T> class _vertex = CEmptyClassTemplate
	>
	class CDataStorage : public CDataStorageDoubleLinkedList<false>::CDataStorage<_data_storage,_vertex> {
	public:
		typedef typename CDataStorageDoubleLinkedList<false>::CDataStorage<
			_data_storage,
			_vertex
		>											inherited;
		typedef typename inherited::inherited_base	inherited_base;
		typedef typename inherited::CGraphVertex	CGraphVertex;
		typedef typename CGraphVertex::_dist_type	_dist_type;
		typedef typename CGraphVertex::_index_type	_index_type;

	protected:
		u32						m_cheap_count;
		bool					m_filled_over;
		CGraphVertex			*m_cheap_list_head;
		CGraphVertex			*m_cheap_list_tail;

	public:
		IC						CDataStorage		(const u32 vertex_count, const _dist_type _max_distance = _dist_type(u32(-1)));
		virtual					~CDataStorage		();
		IC		void			init				();
		IC		bool			is_opened_empty		() const;
		IC		void			verify_cheap		() const;
		IC		void			add_opened			(CGraphVertex &vertex);
		IC		void			add_cheap			(CGraphVertex &vertex);
		IC		void			add_cheap_tail		(CGraphVertex &vertex);
		IC		bool			add_cheap_count		(CGraphVertex &vertex);
		IC		void			remove_cheap_tail	();
		IC		void			fill_cheap			();
		IC		void			decrease_opened		(CGraphVertex &vertex, const _dist_type value);
		IC		void			remove_best_opened	();
		IC		void			add_best_closed		();
		IC		CGraphVertex	&get_best			() const;
	};
};

#include "data_storage_cheap_list_inline.h"