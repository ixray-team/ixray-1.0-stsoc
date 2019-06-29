////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_motivations.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_impl.h"
#include "stalker_decision_space.h"
#include "agent_manager.h"
#include "agent_manager_space.h"
#include "agent_member_manager.h"
#include "script_game_object.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

float CMotivationGlobal::evaluate	(u32 sub_motivation_id)
{
	VERIFY				(m_object);
	if (eMotivationAlive == sub_motivation_id)
		if (object().g_Alive())
			return		(1.f);
		else
			return		(0.f);
	else
		if (eMotivationDead == sub_motivation_id)
			if (!object().g_Alive())
				return	(1.f);
			else
				return	(0.f);

	Debug.fatal			("Couldn't find child motivation!");
	NODEFAULT;
#ifdef DEBUG
	return				(0.f);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CMotivationAlive
//////////////////////////////////////////////////////////////////////////

float CMotivationAlive::evaluate	(u32 sub_motivation_id)
{
	VERIFY				(m_object);
	if (eMotivationSolveZonePuzzle == sub_motivation_id)
		if (m_object->agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeNoOrder)
			return		(1.f);
		else
			return		(0.f);
	else
		if (eMotivationSquadCommand == sub_motivation_id)
			if (m_object->agent_manager().member().member(m_object).order_type() != AgentManager::eOrderTypeNoOrder)
				return	(1.f);
			else
				return	(0.f);

	Debug.fatal			("Couldn't find child motivation!");
	NODEFAULT;
#ifdef DEBUG
	return				(0.f);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CMotivationSquad
//////////////////////////////////////////////////////////////////////////

float CMotivationSquad::evaluate	(u32 sub_motivation_id)
{
	VERIFY				(m_object);
	if (eMotivationSquadGoal == sub_motivation_id)
		if (m_object->agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeGoal)
			return		(1.f);
		else
			return		(0.f);
	else
		if (eMotivationSquadAction == sub_motivation_id)
			if (m_object->agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeAction)
				return	(1.f);
			else
				return	(0.f);

	Debug.fatal			("Couldn't find child motivation!");
	NODEFAULT;
#ifdef DEBUG
	return				(0.f);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CMotivationSquadGoal
//////////////////////////////////////////////////////////////////////////

void CMotivationSquadGoal::update	()
{
	VERIFY				(m_object->agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeGoal);
	m_goal				= m_object->agent_manager().member().member(m_object).goal();
}

