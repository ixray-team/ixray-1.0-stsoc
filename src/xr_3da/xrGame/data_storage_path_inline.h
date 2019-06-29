////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_path_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 02.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Path data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template<bool bEuclidianHeuristics>\
	template <template <typename _T> class _vertex> 


#define CPathDataStorage		CDataStoragePath<bEuclidianHeuristics>::CDataStorage<_vertex>

TEMPLATE_SPECIALIZATION
IC	CPathDataStorage::CDataStorage			(const u32 vertex_count)
{
}

TEMPLATE_SPECIALIZATION
CPathDataStorage::~CDataStorage				()
{
}

TEMPLATE_SPECIALIZATION
IC	void CPathDataStorage::init				()
{
}

TEMPLATE_SPECIALIZATION
IC	void CPathDataStorage::assign_parent	(CGraphVertex	&neighbour, CGraphVertex *parent)
{
	neighbour.back()		= parent;
}

TEMPLATE_SPECIALIZATION
template <typename T>
IC	void CPathDataStorage::assign_parent	(CGraphVertex	&neighbour, CGraphVertex *parent, const T&)
{
	assign_parent			(neighbour,parent);
}

TEMPLATE_SPECIALIZATION
IC	void CPathDataStorage::update_successors(CGraphVertex &tpNeighbour)
{
	NODEFAULT;
}

TEMPLATE_SPECIALIZATION
IC	void CPathDataStorage::get_node_path	(xr_vector<_index_type> &path, CGraphVertex *best)
{
	CGraphVertex			*t1 = best, *t2 = best->back();
	for (u32 i=1; t2; t1 = t2, t2 = t2->back(), ++i) ;

	path.resize				(i);

	t1						= best;
	path[--i]				= best->index();
	t2						= t1->back();

	xr_vector<_index_type>::reverse_iterator	I = path.rbegin();
	xr_vector<_index_type>::reverse_iterator	E = path.rend();
	for (++I ; t2; t2 = t2->back(), ++I)
		*I = t2->index();
}

#undef TEMPLATE_SPECIALIZATION
#undef CPathDataStorage