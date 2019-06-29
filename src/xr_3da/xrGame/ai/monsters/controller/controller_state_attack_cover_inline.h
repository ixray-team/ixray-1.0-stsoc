#pragma once

#include "../states/state_hide_from_point.h"
#include "../states/state_move_to_point.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControllerTakeCoverAbstract CStateControlTakeCover<_Object>

#define STATE_TIME			5000
#define HIDE_TIME			4000
#define GOOD_DIST_TO_START	10.f
#define BAD_DISTANCE		5.f

TEMPLATE_SPECIALIZATION
CStateControllerTakeCoverAbstract::CStateControlTakeCover(_Object *obj) : inherited(obj)
{
	add_state(eStateTakeCover,	xr_new<CStateMonsterHideFromPoint<_Object> >(obj));
	add_state(eStateMoveOut,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateControllerTakeCoverAbstract::~CStateControlTakeCover()
{
}


TEMPLATE_SPECIALIZATION
void CStateControllerTakeCoverAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateControllerTakeCoverAbstract::reselect_state()
{
	if ()
}

TEMPLATE_SPECIALIZATION
bool CStateControllerTakeCoverAbstract::check_start_conditions()
{
	float dist = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist > GOOD_DIST_TO_START) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerTakeCoverAbstract::check_completion()
{
	float dist = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist < BAD_DISTANCE) return true;
	
	if ((object->EnemyMan.see_enemy_now()) && (state_started_time + HIDE_TIME < object->m_current_update)) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateControllerTakeCoverAbstract::finalize()
{
	inherited::finalize();
}

TEMPLATE_SPECIALIZATION
void CStateControllerTakeCoverAbstract::critical_finalize()
{
	inherited::critical_finalize();
}

TEMPLATE_SPECIALIZATION
void CStateControllerTakeCoverAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateTakeCover) {
		SStateHideFromPoint data;

		data.point					= object->EnemyMan.get_enemy_position();
		
		data.accelerated			= true;
		data.braking				= false;
		data.accel_type				= eAT_Aggressive;
		
		data.distance				= ;
		
		data.cover_min_dist			= 10.f;
		data.cover_max_dist			= 40.f;
		data.cover_search_radius	= 15.f;
	
		data.action.action			= ACT_RUN;
		
		data.point					= object->EnemyMan.get_enemy_position();
		data.action.action			= ACT_STAND_IDLE;
		data.action.time_out		= 10000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));
		return;
	}	

	if (current_substate == eStateMoveOut) {
		SStateDataMoveToPoint data;

		data.point				= object->EnemyMan.get_enemy_position();
		data.vertex				= object->EnemyMan.get_enemy_vertex();

		data.accelerated		= true;
		data.accel_type			= eAT_Calm;

		data.completion_dist	= 2.f;

		data.action.action		= ACT_STEAL;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}	
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerTakeCoverAbstract
