////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_path_manager.h"
#include "ai_space.h"
#include "game_graph.h"
#include "graph_search_engine.h"

CGamePathManager::CGamePathManager		()
{
	init					();
}

CGamePathManager::~CGamePathManager		()
{
}

void CGamePathManager::init			()
{
	m_game_path_actual		= true;
	m_game_start_node		= u32(-1);
	m_game_dest_node		= u32(-1);
	m_game_cur_node_index	= u32(-1);
}

bool CGamePathManager::valid			() const
{
	return				(!m_game_path.empty() && (m_game_path[0] == m_game_start_node) && (m_game_path[m_game_path.size() - 1] == m_game_dest_node));
}

bool CGamePathManager::valid			(u32 node_id) const
{
	return				(node_id < ai().game_graph().header().vertex_count());
}

void CGamePathManager::build_game_path	()
{
	if (!m_game_path_actual && valid(m_game_start_node) && valid(m_game_dest_node)) {
		m_game_path_actual		= ai().graph_search_engine().build_path(ai().game_graph(),m_game_start_node,m_game_dest_node,&m_game_path,CGraphSearchEngine::CBaseParameters());
		if (m_game_path_actual)
			m_game_cur_node_index	= 0;
		m_game_path_actual		= true;
	}
}

void CGamePathManager::select_game_path_vertex()
{
}

bool CGamePathManager::game_path_actual() const
{
	return				(true);
}

bool CGamePathManager::game_path_completed() const
{
	return				(true);
}
