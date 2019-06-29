////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "motivation_action_manager_stalker.h"
#include "stalker_motivations.h"
#include "stalker_property_evaluators.h"
#include "stalker_danger_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
#include "script_game_object.h"
#include "stalker_alife_planner.h"
#include "stalker_anomaly_planner.h"
#include "stalker_combat_planner.h"
#include "stalker_death_planner.h"
#include "stalker_danger_planner.h"
#include "stalker_squad_action.h"

//#define GOAP_DEBUG

using namespace StalkerDecisionSpace;

CMotivationActionManagerStalker::CMotivationActionManagerStalker	()
{
	m_affect_cover			= false;
}

CMotivationActionManagerStalker::~CMotivationActionManagerStalker	()
{
}

#ifdef LOG_ACTION
LPCSTR CMotivationActionManagerStalker::action2string	(const _action_id_type &action_id)
{
	return					(inherited::action2string(action_id));
}

LPCSTR CMotivationActionManagerStalker::property2string	(const _condition_type &property_id)
{
	return					(inherited::property2string(property_id));
}

LPCSTR CMotivationActionManagerStalker::object_name		() const
{
	VERIFY					(m_object);
	return					(*m_object->cName());
}
#endif

void CMotivationActionManagerStalker::setup			(CAI_Stalker *object)
{
#ifdef LOG_ACTION
	set_use_log				(!!psAI_Flags.test(aiGOAP));
#endif
	inherited::setup		(object);
	clear					();
	add_motivations			();
	add_evaluators			();
	add_actions				();
	m_affect_cover			= false;
}

void CMotivationActionManagerStalker::update			(u32 time_delta)
{
#ifdef LOG_ACTION
	if ((psAI_Flags.test(aiGOAP) && !m_use_log) || (!psAI_Flags.test(aiGOAP) && m_use_log))
		set_use_log			(!!psAI_Flags.test(aiGOAP));
#endif
	inherited::update		();

#ifdef GOAP_DEBUG
	if (m_failed) {
		{
			Msg			("%d",evaluators().size());
			EVALUATORS::const_iterator	I = evaluators().begin();
			EVALUATORS::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I)
				Msg		("%d,%d",(*I).first,(*I).second->evaluate() ? 1 : 0);
		}
		{
			Msg			("%d",target_state().conditions().size());
			xr_vector<COperatorCondition>::const_iterator	I = target_state().conditions().begin();
			xr_vector<COperatorCondition>::const_iterator	E = target_state().conditions().end();
			for ( ; I != E; ++I)
				Msg		("%d,%d",(*I).condition(),(*I).value() ? 1 : 0);
		}
		{
			Msg			("%d",operators().size());
			const_iterator	I = operators().begin();
			const_iterator	E = operators().end();
			for ( ; I != E; ++I) {
				Msg		("%d,%d",(*I).m_operator_id,(*I).m_operator->weight(target_state(),target_state()));
				{
					Msg		("%d",(*I).m_operator->conditions().conditions().size());
					xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->conditions().conditions().begin();
					xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->conditions().conditions().end();
					for ( ; i != e; ++i)
						Msg	("%d,%d",(*i).condition(),(*i).value() ? 1 : 0);
				}
				{
					Msg		("%d",(*I).m_operator->effects().conditions().size());
					xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->effects().conditions().begin();
					xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->effects().conditions().end();
					for ( ; i != e; ++i)
						Msg	("%d,%d",(*i).condition(),(*i).value() ? 1 : 0);
				}
			}
		}
	}
#endif
}

void CMotivationActionManagerStalker::add_motivations	()
{
	CWorldState				goal;
	
	// complex motivations
	add_motivation			(eMotivationGlobal,			xr_new<CMotivationGlobal>());

	// final motivations
	goal.clear				();
	add_condition			(goal,eWorldPropertyAlreadyDead,	true);
	add_motivation			(eMotivationDead,			xr_new<CStalkerMotivationAction>(goal));

	add_motivation			(eMotivationAlive,			xr_new<CMotivationAlive>());
	
	goal.clear				();
	add_condition			(goal,eWorldPropertyPuzzleSolved,	true);
	add_motivation			(eMotivationSolveZonePuzzle,xr_new<CStalkerMotivationAction>(goal));

	add_motivation			(eMotivationSquadCommand,	xr_new<CMotivationSquad>());

	goal.clear				();
	add_condition			(goal,eWorldPropertySquadGoal,		true);
	add_motivation			(eMotivationSquadGoal,		xr_new<CMotivationSquadGoal>(goal));

	goal.clear				();
	add_condition			(goal,eWorldPropertySquadAction,	true);
	add_motivation			(eMotivationSquadAction,	xr_new<CStalkerMotivationAction>(goal));

	// connect motivations
	add_connection			(eMotivationGlobal,			eMotivationDead);
	add_connection			(eMotivationGlobal,			eMotivationAlive);
	add_connection			(eMotivationAlive,			eMotivationSolveZonePuzzle);
	add_connection			(eMotivationAlive,			eMotivationSquadCommand);
	add_connection			(eMotivationSquadCommand,	eMotivationSquadGoal);
	add_connection			(eMotivationSquadCommand,	eMotivationSquadAction);
}

void CMotivationActionManagerStalker::add_evaluators		()
{
	add_evaluator			(eWorldPropertyAlreadyDead		,xr_new<CStalkerPropertyEvaluatorConst>				(false,"is_already_dead"));
	add_evaluator			(eWorldPropertyPuzzleSolved		,xr_new<CStalkerPropertyEvaluatorConst>				(false,"is_zone_puzzle_solved"));
	add_evaluator			(eWorldPropertySquadAction		,xr_new<CStalkerPropertyEvaluatorConst>				(false,"is_squad_action_assigned"));
	add_evaluator			(eWorldPropertyAlive			,xr_new<CStalkerPropertyEvaluatorAlive>				(m_object,"is_alive"));
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			(m_object,"is_there_enemies",CStalkerCombatPlanner::POST_COMBAT_WAIT_INTERVAL));
	add_evaluator			(eWorldPropertyDanger			,xr_new<CStalkerPropertyEvaluatorDangers>			(m_object,"is_there_danger"));
	add_evaluator			(eWorldPropertyAnomaly			,xr_new<CStalkerPropertyEvaluatorAnomaly>			(m_object,"is_there_anomalies"));
}

void CMotivationActionManagerStalker::add_actions			()
{
	CAction					*action;
	CActionPlannerAction	*planner;

	planner					= xr_new<CStalkerDeathPlanner>(m_object,"death_planner");
	add_condition			(planner,eWorldPropertyAlive,		false);
	add_condition			(planner,eWorldPropertyAlreadyDead,	false);
	add_effect				(planner,eWorldPropertyAlreadyDead,	true);
	add_operator			(eWorldOperatorDeathPlanner,planner);

	planner					= xr_new<CStalkerALifePlanner>(m_object,"alife_planner");
	add_condition			(planner,eWorldPropertyAlive,			true);
	add_condition			(planner,eWorldPropertyEnemy,			false);
	add_condition			(planner,eWorldPropertyAnomaly,			false);
	add_condition			(planner,eWorldPropertyDanger,			false);
	add_condition			(planner,eWorldPropertyPuzzleSolved,	false);
	add_effect				(planner,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorALifePlanner,planner);

	planner					= xr_new<CStalkerCombatPlanner>(m_object,"combat_planner");
	add_condition			(planner,eWorldPropertyAlive,			true);
	add_condition			(planner,eWorldPropertyAnomaly,			false);
	add_condition			(planner,eWorldPropertyEnemy,			true);
	add_effect				(planner,eWorldPropertyEnemy,			false);
	add_operator			(eWorldOperatorCombatPlanner,planner);

	planner					= xr_new<CStalkerDangerPlanner>(m_object,"danger_planner");
	add_condition			(planner,eWorldPropertyAlive,			true);
	add_condition			(planner,eWorldPropertyEnemy,			false);
	add_condition			(planner,eWorldPropertyAnomaly,			false);
	add_condition			(planner,eWorldPropertyDanger,			true);
	add_effect				(planner,eWorldPropertyDanger,			false);
	add_operator			(eWorldOperatorDangerPlanner,planner);

	planner					= xr_new<CStalkerAnomalyPlanner>(m_object,"anomaly_planner");
	add_condition			(planner,eWorldPropertyAlive,		true);
	add_condition			(planner,eWorldPropertyAnomaly,		true);
	add_effect				(planner,eWorldPropertyAnomaly,		false);
	add_operator			(eWorldOperatorAnomalyPlanner,planner);

	action					= xr_new<CStalkerActionSquad>		(m_object,"squad_action");
	add_condition			(action,eWorldPropertyAlive,		true);
	add_condition			(action,eWorldPropertySquadAction,	false);
	add_effect				(action,eWorldPropertySquadAction,	true);
	add_operator			(eWorldOperatorSquadAction,			action);
}
