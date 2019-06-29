////////////////////////////////////////////////////////////////////////////
//	Module 		: action_manager_stalker.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai/stalker/ai_stalker.h"
#include "action_manager_stalker.h"
#include "stalker_property_evaluators.h"
#include "stalker_actions.h"

CActionManagerStalker::CActionManagerStalker	()
{
	init					();
}

CActionManagerStalker::~CActionManagerStalker	()
{
}

void CActionManagerStalker::init				()
{
	// evaluators
	add_evaluators			();
	add_actions				();
}

void CActionManagerStalker::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CActionManagerStalker::reinit				(CAI_Stalker *object)
{
	inherited::reinit				(object,false);
	m_storage[eWorldPropertyDead]	= false;
}

void CActionManagerStalker::reload				(LPCSTR section)
{
	inherited::reload		(section);
}

void CActionManagerStalker::update				(u32 time_delta)
{
	CState					target;
	if (!m_object->g_Alive())
		target.add_condition(CWorldProperty(eWorldPropertyAlreadyDead,true));
	else
		target.add_condition(CWorldProperty(eWorldPropertyMazeSolved,true));

	set_target_state		(target);

	inherited::update		(time_delta);
}

void CActionManagerStalker::add_evaluators		()
{
	add_evaluator			(eWorldPropertyALife		,xr_new<CStalkerPropertyEvaluatorALife>	());
	add_evaluator			(eWorldPropertyAlive		,xr_new<CStalkerPropertyEvaluatorAlive>	());
	add_evaluator			(eWorldPropertyDead			,xr_new<CStalkerPropertyEvaluatorMember>(&m_storage,eWorldPropertyDead,true));
	add_evaluator			(eWorldPropertyAlreadyDead	,xr_new<CStalkerPropertyEvaluatorConst>	(false));
	add_evaluator			(eWorldPropertyMazeSolved	,xr_new<CStalkerPropertyEvaluatorConst>	(false));
}

void CActionManagerStalker::add_actions			()
{
	CAction					*action;

	action					= xr_new<CStalkerActionDead>		(m_object,&m_storage,"dead");
	add_condition			(action,eWorldPropertyAlive,		false);
	add_condition			(action,eWorldPropertyDead,			false);
	add_effect				(action,eWorldPropertyDead,			true);
	add_operator			(eWorldOperatorDead,				action);

	action					= xr_new<CAction>					(m_object,"already_dead");
	add_condition			(action,eWorldPropertyDead,			true);
	add_effect				(action,eWorldPropertyAlreadyDead,	true);
	add_operator			(eWorldOperatorAlreadyDead,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,&m_storage,"free_no_alife");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyALife,		false);
	add_condition			(action,eWorldPropertyMazeSolved,	false);
	add_effect				(action,eWorldPropertyMazeSolved,	true);
	add_operator			(eWorldOperatorFreeNoALife,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,&m_storage,"free_alife");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyMazeSolved,	false);
	add_effect				(action,eWorldPropertyMazeSolved,	true);
	add_operator			(eWorldOperatorFreeALife,			action);
}
