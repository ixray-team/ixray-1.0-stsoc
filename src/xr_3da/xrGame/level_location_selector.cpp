////////////////////////////////////////////////////////////////////////////
//	Module 		: level_location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_location_selector.h"
#include "ai_space.h"
#include "graph_search_engine.h"
#include "entity.h"

CLevelLocationSelector::CLevelLocationSelector		()
{
}

CLevelLocationSelector::~CLevelLocationSelector		()
{
}

//void CLevelLocationSelector::init_evaluator		(PathManagers::CAbstractNodeEvaluator *node_evaluator)
//{
//	CEntity								*l_tpEntity = dynamic_cast<CEntity*>(this);
//	
//	VERIFY								(l_tpEntity);
//	
//	CSquad								&Squad = Level().Teams[l_tpEntity->g_Team()].Squads[l_tpEntity->g_Squad()];
//	CEntity								*Leader = Squad.Leader;
//	if (Leader->g_Health() <= 0)
//		Leader							= l_tpEntity;
//
//	R_ASSERT2							(Leader,"Leader is not assigned!");
//	R_ASSERT2							(ai().level_graph().valid_vertex_id(m_dwLevelVertexID), "Invalid enemy vertex");
//
//	node_evaluator->m_hit_direction		= m_hit_direction;
//	node_evaluator->m_hit_time			= m_hit_time;
//	node_evaluator->m_dwCurTime			= Device.dwTimeGlobal;
//	node_evaluator->m_tMe				= l_tpEntity;
//	node_evaluator->m_tpMyNode			= m_tpLevelVertex;
//	node_evaluator->m_tMyPosition		= Position();
//	node_evaluator->m_tEnemy			= m_selected_enemy.m_enemy;
//	node_evaluator->m_tEnemyPosition	= m_selected_enemy.m_enemy->Position();
//	node_evaluator->m_dwEnemyNode		= m_dwLevelVertexID;
//	node_evaluator->m_tpEnemyNode		= m_selected_enemy.m_enemy->m_tpLevelVertex;
//	node_evaluator->m_taMembers			= &(Squad.Groups[l_tpEntity->g_Group()].Members);
//	node_evaluator->m_dwStartNode		= m_dwLevelVertexID;
//	node_evaluator->m_tStartPosition	= Position();
//}
//
//template <u64 flags>
//void CLevelLocationSelector::select_location(PathManagers::CVertexEvaluator<flags> *node_evaluator)
//{
//	CEntity						*l_tpEntity = dynamic_cast<CEntity*>(this);
//	VERIFY						(l_tpEntity);
//	CSquad						&Squad = Level().Teams[l_tpEntity->g_Team()].Squads[l_tpEntity->g_Squad()];
//	CEntity						*Leader = Squad.Leader;
//	if (Leader->g_Health() <= 0)
//		Leader					= l_tpEntity;
//	
//	R_ASSERT					(Leader);
//
//	Device.Statistic.AI_Range.Begin();
//
//	m_dwLastLocationSelectionTime = node_evaluator->m_dwCurTime;
//	
//	float l_old_cost			= dInfinity;
//	
//	if (m_level_dest_node != u32(-1)) {
//		node_evaluator->m_tpCurrentNode	= ai().level_graph().vertex(m_level_dest_node);
//		node_evaluator->m_fDistance		= Position().distance_to(ai().level_graph().vertex_position(m_level_dest_node));
//		l_old_cost						= node_evaluator->ffEvaluateNode();
//	}
//
//	Squad.Groups[l_tpEntity->g_Group()].GetAliveMemberInfo(node_evaluator->m_taMemberPositions, node_evaluator->m_taMemberNodes, node_evaluator->m_taDestMemberPositions, node_evaluator->m_taDestMemberNodes, l_tpEntity);
//	
//	ai().graph_search_engine().build_path(ai().level_graph(),m_dwLevelVertexID,m_dwLevelVertexID,0,*node_evaluator);
//
//	if (node_evaluator->m_fBestCost < l_old_cost)
//		m_selector_failed		= false;
//	else
//		m_selector_failed		= true;
//
//	Device.Statistic.AI_Range.End();


void CGameLocationSelector::perform_search		(const ALife::_GRAPH_ID game_vertex_id)
{
	VERIFY				(m_game_selector_evaluator);
	ai().graph_search_engine().build_path(ai().level_graph(),game_vertex_id,game_vertex_id,0,m_game_selector_evaluator);
	m_game_selector_failed	= !ai().game_graph().valid_vertex_id(m_game_selector_evaluator->m_vertex_id) || (m_game_selector_evaluator->m_vertex_id == m_selected_game_vertex_id);
}

void CLevelLocationSelector::select_level_vertex()
{
	if (!m_game_selector_failed) {
		perform_search	(game_vertex_id);
		return;
	}
}

bool CLevelLocationSelector::level_vertex_selection_actual()
{
	perform_search		(game_vertex_id);
	return				(m_game_selector_failed);
}
