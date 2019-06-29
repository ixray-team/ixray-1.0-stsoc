////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_dialog_planner.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife dialog planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_dialog_planner.h"
#include "stalker_alife_dialog_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

using namespace StalkerDecisionSpace;

CStalkerALifeDialogPlanner::CStalkerALifeDialogPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited								(object,action_name)
{
}

CStalkerALifeDialogPlanner::~CStalkerALifeDialogPlanner	()
{
}

void CStalkerALifeDialogPlanner::setup			(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);

	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerALifeDialogPlanner::initialize		()
{
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyReadyToDialog,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyHelloCompleted,false);
}

void CStalkerALifeDialogPlanner::add_evaluators	()
{
	add_evaluator			(eWorldPropertyHumanToDialog			,xr_new<CStalkerPropertyEvaluatorHumanToDialog>				());
	add_evaluator			(eWorldPropertyReadyToDialog			,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyReadyToDialog,true,true));
	add_evaluator			(eWorldPropertyHelloCompleted			,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyHelloCompleted,true,true));
}

void CStalkerALifeDialogPlanner::add_actions		()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionGetReadyToDialog>	(m_object,"action_get_ready_to_dialog");
	add_condition			(action,eWorldPropertyReadyToDialog,false);
	add_effect				(action,eWorldPropertyReadyToDialog,true);
	add_operator			(eWorldOperatorGetReadyToDialog,	action);

	action					= xr_new<CStalkerActionHello>				(m_object,"action_hello");
	add_condition			(action,eWorldPropertyReadyToDialog,true);
	add_condition			(action,eWorldPropertyHelloCompleted,false);
	add_effect				(action,eWorldPropertyHelloCompleted,true);
	add_operator			(eWorldOperatorHello,	action);

	action					= xr_new<CStalkerActionDialog>				(m_object,"action_dialog");
	add_condition			(action,eWorldPropertyReadyToDialog,true);
	add_condition			(action,eWorldPropertyHelloCompleted,true);
	add_condition			(action,eWorldPropertyHumanToDialog,true);
	add_effect				(action,eWorldPropertyHumanToDialog,false);
	add_operator			(eWorldOperatorDialog,	action);
}
