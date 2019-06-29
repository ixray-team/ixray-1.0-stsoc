////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_guard_planner.cpp
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker guard planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_guard_planner.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_space.h"
#include "sound_player.h"
#include "script_game_object.h"
#include "stalker_guard_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

CStalkerGuardPlanner::CStalkerGuardPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited									(object,action_name)
{
}

CStalkerGuardPlanner::~CStalkerGuardPlanner	()
{
}

void CStalkerGuardPlanner::setup				(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);

	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerGuardPlanner::finalize			()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().sound().remove_active_sounds		(u32(-1));
}

void CStalkerGuardPlanner::initialize		()
{
	inherited::initialize	();
	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerGuardPlanner::execute			()
{
	inherited::execute		();
}

void CStalkerGuardPlanner::add_evaluators		()
{
	add_evaluator			(eWorldPropertyDanger			,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger"));
	add_evaluator			(eWorldPropertyDangerRicochet	,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : ricochet"	,CDangerObject::eDangerTypeRicochet));
	add_evaluator			(eWorldPropertyDangerShot		,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : shot"		,CDangerObject::eDangerTypeShot));
	add_evaluator			(eWorldPropertyDangerHit		,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : hit"		,CDangerObject::eDangerTypeHit));
	add_evaluator			(eWorldPropertyDangerDeath		,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : death"		,CDangerObject::eDangerTypeDeath));
	add_evaluator			(eWorldPropertyDangerAttack		,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : attack"	,CDangerObject::eDangerTypeAttack));
	add_evaluator			(eWorldPropertyDangerCorpse		,xr_new<CStalkerPropertyEvaluatorDangers>	(m_object,"is_there_danger : corpse"	,CDangerObject::eDangerTypeCorpse));
}

void CStalkerGuardPlanner::add_actions			()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionDangerRicochet>	(m_object,"action_danger_ricochet");
	add_condition			(action,eWorldPropertyDangerRicochet,true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerRicochet,		action);

	action					= xr_new<CStalkerActionDangerShot>		(m_object,"action_danger_shot");
	add_condition			(action,eWorldPropertyDangerShot,	true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerShot,			action);

	action					= xr_new<CStalkerActionDangerHit>		(m_object,"action_danger_hit");
	add_condition			(action,eWorldPropertyDangerHit,	true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerHit,			action);

	action					= xr_new<CStalkerActionDangerDeath>		(m_object,"action_danger_death");
	add_condition			(action,eWorldPropertyDangerDeath,	true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerDeath,		action);

	action					= xr_new<CStalkerActionDangerAttack>	(m_object,"action_danger_attack");
	add_condition			(action,eWorldPropertyDangerAttack,	true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerAttack,		action);

	action					= xr_new<CStalkerActionDangerCorpse>	(m_object,"action_danger_corpse");
	add_condition			(action,eWorldPropertyDangerCorpse,	true);
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerCorpse,		action);
}
