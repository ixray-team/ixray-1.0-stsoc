////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_planner_missile.cpp
//	Created 	: 11.03.2004
//  Modified 	: 01.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler action planner missile handling
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_handler_planner.h"
#include "object_property_evaluators.h"
#include "object_actions.h"
#include "object_handler_space.h"
#include "fooditem.h"
#include "object_handler_planner_impl.h"
#include "ai/stalker/ai_stalker.h"

using namespace ObjectHandlerSpace;

void CObjectHandlerPlanner::add_evaluators		(CFoodItem *food_item)
{
	u16					id = food_item->ID();
	// dynamic state properties
	add_evaluator		(uid(id,eWorldPropertyHidden)		,xr_new<CObjectPropertyEvaluatorFood>(food_item,m_object,FOOD_HIDDEN));
	add_evaluator		(uid(id,eWorldPropertyPrepared)		,xr_new<CObjectPropertyEvaluatorPrepared>(food_item,m_object));
	add_evaluator		(uid(id,eWorldPropertyUseEnough)	,xr_new<CObjectPropertyEvaluatorMember>(&m_storage,eWorldPropertyUseEnough,true));
	
	// const properties
	add_evaluator		(uid(id,eWorldPropertyDropped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyIdle)			,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyUsed)			,xr_new<CObjectPropertyEvaluatorConst>(false));
	add_evaluator		(uid(id,eWorldPropertyStrapped)		,xr_new<CObjectPropertyEvaluatorConst>(false));
}

void CObjectHandlerPlanner::add_operators		(CFoodItem *food_item)
{
	u16					id = food_item->ID(), ff = u16(-1);
	CActionBase<CAI_Stalker>	*action;

	// show
	action				= xr_new<CObjectActionShow>(food_item,m_object,&m_storage,"show");
	add_condition		(action,id,eWorldPropertyHidden,	true);
	add_condition		(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,id,eWorldPropertyHidden,	false);
	add_operator		(uid(id,eWorldOperatorShow),		action);

	// hide
	action				= xr_new<CObjectActionHide>(food_item,m_object,&m_storage,"hide");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,ff,eWorldPropertyItemID,	false);
	add_effect			(action,ff,eWorldPropertyItemID,	true);
	add_effect			(action,id,eWorldPropertyHidden,	true);
	add_operator		(uid(id,eWorldOperatorHide),		action);

	// drop
	action				= xr_new<CObjectActionDrop>(food_item,m_object,&m_storage,"drop");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyDropped,	true);
	add_operator		(uid(id,eWorldOperatorDrop),		action);

	// idle
	action				= xr_new<CObjectActionIdle>(food_item,m_object,&m_storage,"idle");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_effect			(action,id,eWorldPropertyIdle,		true);
	add_effect			(action,id,eWorldPropertyUseEnough,	false);
	add_effect			(action,id,eWorldPropertyStrapped,	true);
	add_operator		(uid(id,eWorldOperatorIdle),		action);

	// prepare
	action				= xr_new<CObjectActionPrepare>(food_item,m_object,&m_storage,"prepare");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyPrepared,	false);
	add_effect			(action,id,eWorldPropertyPrepared,	true);
	add_operator		(uid(id,eWorldOperatorPrepare),		action);

	// action
	action				= xr_new<CObjectActionUse>(food_item,m_object,&m_storage,"use");
	add_condition		(action,id,eWorldPropertyHidden,	false);
	add_condition		(action,id,eWorldPropertyPrepared,	true);
	add_condition		(action,id,eWorldPropertyUsed,		false);
	add_condition		(action,id,eWorldPropertyUseEnough,	false);
	add_effect			(action,id,eWorldPropertyUsed,		true);
	add_operator		(uid(id,eWorldOperatorUse),			action);
}
