////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_motivations.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager motivations
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation.h"
#include "motivation_action.h"
//#include "script_game_object.h"

class CAgentManager;

typedef CMotivation<CAgentManager>			CAgentManagerMotivation;
typedef CMotivationAction<CAgentManager>	CAgentManagerMotivationAction;

#include "agent_manager_motivations_inline.h"