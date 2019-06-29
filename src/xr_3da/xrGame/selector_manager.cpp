////////////////////////////////////////////////////////////////////////////
//	Module 		: selector_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Selector manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "selector_manager.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "custommonster.h"
#include "memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "memory_space.h"
#include "ai_object_location.h"
#include "level.h"

CSelectorManager::CSelectorManager		(CCustomMonster *object)
{
	VERIFY						(object);
	m_object					= object;
}

CSelectorManager::~CSelectorManager		()
{
	remove_all					();
}

void CSelectorManager::reinit			()
{
	remove_all					();
}

void CSelectorManager::remove			(LPCSTR selector_name)
{
	SELECTORS::iterator			I = m_selectors.find(selector_name);
	VERIFY						(m_selectors.end() != I);
	xr_delete					((*I).second);
	m_selectors.erase			(I);
}

void CSelectorManager::remove_all		()
{
	while (!m_selectors.empty())
		remove					(m_selectors.begin()->first);
}

void CSelectorManager::init_selector	(CAbstractVertexEvaluator &S)
{
	const CEntityAlive		*entity_alive = m_object;

	/**
	if (m_object->memory().hit().hit()) {
		S.m_hit_direction	= m_object->memory().hit().hit()->m_direction;
		S.m_hit_time		= m_object->memory().hit().hit()->m_level_time;
	}
	else {
		S.m_hit_direction	= Fvector().set(0,0,0);
		S.m_hit_time		= 0;
	}
	/**/

	S.m_dwCurTime			= Device.dwTimeGlobal;
	S.m_tMe					= smart_cast<CEntity*>(this);
	S.m_tpMyNode			= m_object->ai_location().level_vertex();
	S.m_tMyPosition			= m_object->Position();
	
	if (m_object->memory().enemy().selected()) {
		S.m_tEnemy			= m_object->memory().enemy().selected();
		S.m_tEnemyPosition	= m_object->memory().enemy().selected()->Position();
		S.m_dwEnemyNode		= m_object->memory().enemy().selected()->ai_location().level_vertex_id();
		S.m_tpEnemyNode		= m_object->memory().enemy().selected()->ai_location().level_vertex();
	}
	else {
		S.m_tEnemy			= 0;
		S.m_dwEnemyNode		= u32(-1);
		S.m_tpEnemyNode		= 0;
	}
	
	S.m_taMembers			= &(Level().seniority_holder().team(entity_alive->g_Team()).squad(entity_alive->g_Squad()).group(entity_alive->g_Group()).members());
	S.m_dwStartNode			= m_object->ai_location().level_vertex_id();
	S.m_tStartPosition		= m_object->Position();
}
