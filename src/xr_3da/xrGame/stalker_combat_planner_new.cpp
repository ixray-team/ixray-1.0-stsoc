////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_planner_new.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat planner new(!?)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_combat_planner_new.h"
#include "stalker_combat_action_new.h"
#include "stalker_danger_property_evaluators.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/stalker/ai_stalker_impl.h"
#include "script_game_object.h"
#include "cover_evaluators.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "stalker_movement_restriction.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "enemy_manager.h"
#include "danger_manager.h"
#include "sound_player.h"
#include "missile.h"
#include "explosive.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "member_order.h"
#include "ai/stalker/ai_stalker_space.h"
#include "stalker_planner.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

CStalkerCombatPlannerNew::CStalkerCombatPlannerNew	(CAI_Stalker *object, LPCSTR action_name) :
	inherited									(object,action_name)
{
}

CStalkerCombatPlannerNew::~CStalkerCombatPlannerNew	()
{
	CAI_Stalker::on_best_cover_changed_delegate	temp;
	temp.bind									(this,&CStalkerCombatPlannerNew::on_best_cover_changed);
	object().unsubscribe_on_best_cover_changed	(temp);
}

void CStalkerCombatPlannerNew::on_best_cover_changed(const CCoverPoint *new_cover, const CCoverPoint *old_cover)
{
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,	false);
}

void CStalkerCombatPlannerNew::setup				(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup			(object,storage);

	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,				false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyUseSuddenness,		true);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyUseCrouchToLookOut,	true);
	
	clear					();
	add_evaluators			();
	add_actions				();

	CAI_Stalker::on_best_cover_changed_delegate		temp;
	temp.bind										(this,&CStalkerCombatPlannerNew::on_best_cover_changed);
	this->object().subscribe_on_best_cover_changed	(temp);
}

void CStalkerCombatPlannerNew::execute				()
{
	if (first_time())
		object().agent_manager().member().register_in_combat(m_object);

	inherited::execute		();
}

void CStalkerCombatPlannerNew::update				()
{
	if (object().memory().enemy().selected()) {
		CMemoryInfo					mem_object = object().memory().memory(object().memory().enemy().selected());

		if (mem_object.m_object) {
			object().best_cover		(mem_object.m_object_params.m_position);
		}
	}

	inherited::update		();
	object().react_on_grenades		();
	object().react_on_member_death	();
}

void CStalkerCombatPlannerNew::initialize			()
{
	inherited::initialize	();
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyInCover,			false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedOut,		false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyPositionHolded,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyEnemyDetoured,	false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyUseSuddenness,	true);

	object().agent_manager().member().member(m_object).cover(0);
	m_last_enemy_id			= u16(-1);
	m_last_level_time		= 0;

	if (object().memory().enemy().selected()) {
		CVisualMemoryManager	*visual_memory_manager = object().memory().enemy().selected()->visual_memory();
		VERIFY					(visual_memory_manager);
		CScriptActionPlanner::m_storage.set_property(eWorldPropertyUseSuddenness,	!visual_memory_manager->visible_now(&object()));
	}

	//.
	if (object().agent_manager().member().members().size() > 1)
		CScriptActionPlanner::m_storage.set_property	(eWorldPropertyUseSuddenness,	false);

	if (m_object->memory().visual().visible_now(m_object->memory().enemy().selected()) && m_object->memory().enemy().selected()->human_being())
		if (!CScriptActionPlanner::m_storage.property(eWorldPropertyUseSuddenness))
			object().sound().play						(eStalkerSoundAlarm);
}

void CStalkerCombatPlannerNew::finalize			()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().memory().danger().time_line(Device.dwTimeGlobal + 3000);

//	object().sound().remove_active_sounds					(eStalkerSoundMaskNoDanger);
}

void CStalkerCombatPlannerNew::add_evaluators		()
{
	add_evaluator			(eWorldPropertyPureEnemy		,xr_new<CStalkerPropertyEvaluatorEnemies>			(m_object,"is_there_enemies",0));
	add_evaluator			(eWorldPropertyEnemy			,xr_new<CStalkerPropertyEvaluatorEnemies>			(m_object,"is_there_enemies_delayed",POST_COMBAT_WAIT_INTERVAL));
	add_evaluator			(eWorldPropertySeeEnemy			,xr_new<CStalkerPropertyEvaluatorSeeEnemy>			(m_object,"see enemy"));
	add_evaluator			(eWorldPropertyItemToKill		,xr_new<CStalkerPropertyEvaluatorItemToKill>		(m_object,"item to kill"));
	add_evaluator			(eWorldPropertyItemCanKill		,xr_new<CStalkerPropertyEvaluatorItemCanKill>		(m_object,"item can kill"));
	add_evaluator			(eWorldPropertyFoundItemToKill	,xr_new<CStalkerPropertyEvaluatorFoundItemToKill>	(m_object,"found item to kill"));
	add_evaluator			(eWorldPropertyFoundAmmo		,xr_new<CStalkerPropertyEvaluatorFoundAmmo>			(m_object,"found ammo"));
	add_evaluator			(eWorldPropertyReadyToKill		,xr_new<CStalkerPropertyEvaluatorReadyToKill>		(m_object,"ready to kill"));
	add_evaluator			(eWorldPropertyPanic			,xr_new<CStalkerPropertyEvaluatorPanic>				(m_object,"panic"));
	add_evaluator			(eWorldPropertyDangerGrenade	,xr_new<CStalkerPropertyEvaluatorGrenadeToExplode>	(m_object,"is there grenade to explode"));
	add_evaluator			(eWorldPropertyInCover			,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyInCover,true,true,"in cover"));
	add_evaluator			(eWorldPropertyLookedOut		,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyLookedOut,true,true,"looked out"));
	add_evaluator			(eWorldPropertyPositionHolded	,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyPositionHolded,true,true,"position holded"));
	add_evaluator			(eWorldPropertyEnemyDetoured	,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyEnemyDetoured,true,true,"enemy detoured"));
	add_evaluator			(eWorldPropertyUseSuddenness	,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyUseSuddenness,true,true,"use suddenness"));
}

void CStalkerCombatPlannerNew::add_actions			()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionGetItemToKill>	(m_object,"get_item_to_kill");
	add_condition			(action,eWorldPropertyFoundItemToKill,	true);
	add_condition			(action,eWorldPropertyItemToKill,		false);
	add_effect				(action,eWorldPropertyItemToKill,		true);
	add_effect				(action,eWorldPropertyItemCanKill,		true);
	add_operator			(eWorldOperatorGetItemToKill,			action);

	action					= xr_new<CStalkerActionMakeItemKilling>	(m_object,"make_item_killing");
	add_condition			(action,eWorldPropertyFoundAmmo,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	false);
	add_effect				(action,eWorldPropertyItemCanKill,	true);
	add_operator			(eWorldOperatorMakeItemKilling,		action);

	action					= xr_new<CStalkerActionRetreatFromEnemyNew>(m_object,"retreat_from_enemy_new");
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorRetreatFromEnemy,	action);

	action					= xr_new<CStalkerActionGetReadyToKillNew>	(m_object,"get_ready_to_kill");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	false);
	add_effect				(action,eWorldPropertyReadyToKill,	true);
	add_operator			(eWorldOperatorGetReadyToKill,			action);

	action					= xr_new<CStalkerActionKillEnemy>		(m_object,"kill_enemy");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyPanic,		false);
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorKillEnemy,				action);

	action					= xr_new<CStalkerActionTakeCoverNew>	(m_object,"take_cover");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyItemToKill,	true);
	add_condition			(action,eWorldPropertyItemCanKill,	true);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_effect				(action,eWorldPropertyInCover,		true);
	add_operator			(eWorldOperatorTakeCover,				action);

	action					= xr_new<CStalkerActionLookOut>			(m_object,"look_out");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyLookedOut,	false);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_effect				(action,eWorldPropertyLookedOut,	true);
	add_operator			(eWorldOperatorLookOut,					action);

	action					= xr_new<CStalkerActionHoldPosition>	(m_object,"hold_position");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		true);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyPositionHolded,false);
	add_effect				(action,eWorldPropertyInCover,		false);
	add_effect				(action,eWorldPropertyPositionHolded,true);
	add_operator			(eWorldOperatorHoldPosition,			action);

	action					= xr_new<CStalkerActionDetourEnemy>		(m_object,"detour_enemy");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_condition			(action,eWorldPropertyEnemyDetoured,false);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertyPositionHolded,true);
	add_effect				(action,eWorldPropertyEnemyDetoured,true);
	add_operator			(eWorldOperatorDetourEnemy,			action);

	action					= xr_new<CStalkerActionSearchEnemy>		(m_object,"search_enemy");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,false);
	add_condition			(action,eWorldPropertyReadyToKill,	true);
	add_condition			(action,eWorldPropertySeeEnemy,		false);
	add_condition			(action,eWorldPropertyInCover,		false);
	add_condition			(action,eWorldPropertyLookedOut,	true);
	add_condition			(action,eWorldPropertyPositionHolded,true);
	add_condition			(action,eWorldPropertyEnemyDetoured,true);
	add_condition			(action,eWorldPropertyPanic,		false);
	add_effect				(action,eWorldPropertyPureEnemy,	false);
	add_operator			(eWorldOperatorSearchEnemy,			action);
	action->set_inertia_time(120000);

	action					= xr_new<CStalkerActionHideFromGrenade>	(m_object,"hide from grenade");
	add_condition			(action,eWorldPropertyDangerGrenade,true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorHideFromGrenade,		action);

	action					= xr_new<CStalkerActionSuddenAttack>	(m_object,"sudden attack");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyUseSuddenness,true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorSuddenAttack,		action);

	action					= xr_new<CStalkerActionPostCombatWait>	(m_object,"post_combat_wait");
	add_condition			(action,eWorldPropertyDangerGrenade,false);
	add_condition			(action,eWorldPropertyPureEnemy,	false);
	add_condition			(action,eWorldPropertyEnemy,		true);
	add_effect				(action,eWorldPropertyEnemy,		false);
	add_operator			(eWorldOperatorPostCombatWait,		action);
}
