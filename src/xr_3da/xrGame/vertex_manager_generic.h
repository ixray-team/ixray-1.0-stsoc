////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_manager_generic.h
//	Created 	: 21.03.2002
//  Modified 	: 01.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Generic vertex manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _path_id_type,
	typename _index_type
>
struct CVertexManagerGeneric {

	template <template <typename _T> class T1>
	struct VertexManager {
		template<typename T2>
		struct _vertex : public T1<T2> {
			typedef _index_type _index_type;
			_index_type	_index;
			bool		_opened;

			IC	const _index_type &index() const
			{
				return	(_index);
			}

			IC	_index_type &index()
			{
				return	(_index);
			}

			IC	bool &opened()
			{
				return	(_opened);
			}

			IC	bool opened() const
			{
				return	(_opened);
			}
		};
	};

	template <
		template <typename _T> class _vertex = CEmptyClassTemplate,
		template <typename _T1, typename _T2> class _index_vertex = CEmptyClassTemplate2,
		typename _data_storage = CBuilderAllocatorConstructor
	> 
	class CDataStorage : public _data_storage::CDataStorage<VertexManager<_vertex>::_vertex> {
	public:
		typedef typename _data_storage::CDataStorage<
			VertexManager<
				_vertex
			>::_vertex
		>												inherited;
		typedef typename inherited::CGraphVertex		CGraphVertex;
		typedef typename CGraphVertex::_index_type		_index_type;

#pragma pack(push,1)
		template <typename _path_id_type>
		struct SGraphIndexVertex : public _index_vertex<CGraphVertex,SGraphIndexVertex> {
			CGraphVertex	*m_vertex;
		};
#pragma pack(pop)

		typedef _path_id_type							_path_id_type;
		typedef SGraphIndexVertex<_path_id_type>		CGraphIndexVertex;

		struct CComparePredicate {
			IC	bool operator()	(const _index_type *vertex0, const _index_type *vertex1) const
			{
				return		(*vertex0 < *vertex1);
			}
		};

		typedef xr_map<const _index_type*,CGraphIndexVertex,CComparePredicate> _index_map;

	protected:
		_path_id_type			m_current_path_id;
		_index_map				m_indexes;

	public:
		IC						CDataStorage	(const u32 vertex_count);
		virtual					~CDataStorage	();
		IC		void			init			();
		IC		bool			is_opened		(const CGraphVertex &vertex) const;
		IC		bool			is_visited		(const _index_type &vertex_id) const;
		IC		bool			is_closed		(const CGraphVertex &vertex) const;
		IC		CGraphVertex	&get_node		(const _index_type &vertex_id) const;
		IC		CGraphVertex	&create_vertex	(CGraphVertex &vertex, const _index_type &vertex_id);
		IC		void			add_opened		(CGraphVertex &vertex);
		IC		void			add_closed		(CGraphVertex &vertex);
		IC		_path_id_type	current_path_id	() const;
	};
};

#include "vertex_manager_generic_inline.h"