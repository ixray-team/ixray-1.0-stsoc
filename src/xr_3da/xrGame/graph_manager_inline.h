////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_manager_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "graph_engine.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Data,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _vertex_index_type\
>

#define CAbstractGraphManager CGraphManagerAbstract<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>

TEMPLATE_SPECIALIZATION
IC	CAbstractGraphManager::CGraphManagerAbstract	()
{
	init					();
}

TEMPLATE_SPECIALIZATION
CAbstractGraphManager::~CGraphManagerAbstract		()
{
	xr_delete				(m_graph);
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::init					()
{
	m_graph					= xr_new<CSGraphAbstract>();
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::reinit					(bool clear_all)
{
	if (clear_all) {
		xr_delete			(m_graph);
		m_graph				= xr_new<CSGraphAbstract>();
	}
	m_current_vertex_id		= m_dest_vertex_id = _vertex_id_type(-1);
	m_path.clear			();
	m_actuality				= true;
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraphManager::set_dest_vertex_id	(const _vertex_id_type dest_vertex_id)
{
	VERIFY					(graph().vertex(dest_vertex_id));
	m_actuality				= m_actuality && (m_dest_vertex_id == dest_vertex_id);
	m_dest_vertex_id		= dest_vertex_id;
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraphManager::set_current_vertex_id	(const _vertex_id_type current_vertex_id)
{
	VERIFY					(graph().vertex(current_vertex_id));
	m_actuality				= m_actuality && (m_current_vertex_id == current_vertex_id);
	m_current_vertex_id		= current_vertex_id;
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_id_type &CAbstractGraphManager::dest_vertex_id() const
{
	return					(m_dest_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_id_type &CAbstractGraphManager::current_vertex_id() const
{
	return					(m_current_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractGraphManager::actual	() const
{
	return					(m_actuality);
}

TEMPLATE_SPECIALIZATION
void CAbstractGraphManager::update	(u32 time_delta)
{
	if (m_actuality)
		return;
	m_actuality				= true;
	m_path.clear			();
#ifdef DEBUG
	bool					successfull = ai().graph_engine().search(graph(),graph().vertex_index(current_vertex_id()),graph().vertex_index(dest_vertex_id()),&m_path,CGraphEngine::CBaseParameters());
	VERIFY					(successfull);
	VERIFY					(!m_path.empty());
#endif

	xr_vector<u32>::iterator I = m_path.begin();
	xr_vector<u32>::iterator E = m_path.end();
	for ( ; I != E; ++I)
		*I					= graph().vertices()[*I].vertex_id();

	VERIFY					(dest_vertex_id() == m_path.back());

	if (m_path.size() == 1)
		m_path.clear();
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<_vertex_id_type> &CAbstractGraphManager::path() const
{
	return					(m_path);
}

TEMPLATE_SPECIALIZATION
IC	xr_vector<_vertex_id_type> &CAbstractGraphManager::path()
{
	return					(m_path);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraphManager::go_path()
{
	VERIFY					(!path().empty());
	m_path.erase			(m_path.begin());
	if (!m_path.empty())
		m_current_vertex_id	= m_path.front();
}

TEMPLATE_SPECIALIZATION
IC	const typename CAbstractGraphManager::CSGraphAbstract &CAbstractGraphManager::graph	() const
{
	VERIFY			(m_graph);
	return			(*m_graph);
}

TEMPLATE_SPECIALIZATION
IC	typename CAbstractGraphManager::CSGraphAbstract &CAbstractGraphManager::graph		()
{
	VERIFY			(m_graph);
	return			(*m_graph);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractGraphManager