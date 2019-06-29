////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "agent_corpse_manager.h"
#include "agent_enemy_manager.h"
#include "agent_explosive_manager.h"
#include "agent_location_manager.h"
#include "agent_member_manager.h"
#include "agent_memory_manager.h"
#include "agent_manager_planner.h"
#include "profiler.h"

CAgentManager::CAgentManager			()
{
	init_scheduler				();
	init_components				();
}

CAgentManager::~CAgentManager			()
{
	VERIFY						(member().members().empty());
	remove_scheduler			();
	remove_components			();
}

void CAgentManager::init_scheduler		()
{
	shedule.t_min				= 1000;
	shedule.t_max				= 1000;
	shedule_register			();
}

void CAgentManager::init_components		()
{
	m_corpse					= xr_new<CAgentCorpseManager>	(this);
	m_enemy						= xr_new<CAgentEnemyManager>	(this);
	m_explosive					= xr_new<CAgentExplosiveManager>(this);
	m_location					= xr_new<CAgentLocationManager>	(this);
	m_member					= xr_new<CAgentMemberManager>	(this);
	m_memory					= xr_new<CAgentMemoryManager>	(this);
	m_brain						= xr_new<CAgentManagerPlanner>	();
	brain().setup				(this);
}

void CAgentManager::remove_scheduler	()
{
	shedule_unregister			();
}

void CAgentManager::remove_components	()
{
	xr_delete					(m_corpse);
	xr_delete					(m_enemy);
	xr_delete					(m_explosive);
	xr_delete					(m_location);
	xr_delete					(m_member);
	xr_delete					(m_memory);
	xr_delete					(m_brain);
}

void CAgentManager::remove_links		(CObject *object)
{
	corpse().remove_links		(object);
	enemy().remove_links		(object);
	explosive().remove_links	(object);
	location().remove_links		(object);
	member().remove_links		(object);
	memory().remove_links		(object);
	brain().remove_links		(object);
}

void CAgentManager::shedule_Update		(u32 time_delta)
{
	START_PROFILE("Agent_Manager")
	ISheduled::shedule_Update	(time_delta);

	if (!member().members().empty()) {
		memory().update			();
		corpse().update			();
		enemy().update			();
		explosive().update		();
		location().update		();
		member().update			();
		brain().update			();
		return;
	}

	CAgentManager				*self = this;
	xr_delete					(self);
	STOP_PROFILE
}

float CAgentManager::shedule_Scale		()
{
	return						(.5f);
}
