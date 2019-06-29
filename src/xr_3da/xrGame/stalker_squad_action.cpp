////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_squad_action.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker squad action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_squad_action.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "script_game_object.h"
#include "setup_manager.h"
#include "setup_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSquad
//////////////////////////////////////////////////////////////////////////

CStalkerActionSquad::CStalkerActionSquad	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionSquad::initialize		()
{
	inherited::initialize	();
	
	Msg						("AgentManager::eOrderTypeNoOrder = %d",AgentManager::eOrderTypeNoOrder);
	Msg						("AgentManager::eOrderTypeAction  = %d",AgentManager::eOrderTypeAction);
	Msg						("AgentManager::eOrderTypeGoal    = %d",AgentManager::eOrderTypeGoal);
	Msg						("AgentManager::eOrderTypeDummy   = %d",AgentManager::eOrderTypeDummy);
	Msg						("unknown                         = %d",object().agent_manager().member().member(m_object).order_type());
	
	VERIFY					(object().agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeAction);
	object().setup().clear();
	object().setup().add_action(0,xr_new<CSetupAction>(object().agent_manager().member().member(m_object).action()));
}

void CStalkerActionSquad::execute			()
{
	inherited::execute		();
	VERIFY					(object().agent_manager().member().member(m_object).order_type() == AgentManager::eOrderTypeAction);
	object().setup().action(0).update(object().agent_manager().member().member(m_object).action());
}

void CStalkerActionSquad::finalize			()
{
	inherited::finalize		();
	object().setup().clear	();
}
