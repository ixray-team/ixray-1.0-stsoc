#pragma once

#include "cat_state_attack_rat_melee.h"
#include "cat_state_attack_rat_run.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateCatAttackRatAbstract CStateCatAttackRat<_Object>

TEMPLATE_SPECIALIZATION
CStateCatAttackRatAbstract::CStateCatAttackRat(_Object *obj) : inherited(obj)
{
	add_state	(eStateRun,		xr_new<CStateCatAttackRatRun<_Object> >(obj));
	add_state	(eStateMelee,	xr_new<CStateCatAttackRatMelee<_Object> >(obj));
}


TEMPLATE_SPECIALIZATION
void CStateCatAttackRatAbstract::execute()
{
	if ((prev_substate == u32(-1)) || (prev_substate == eStateRun)) {
		if (get_state(eStateMelee)->check_start_conditions())
			select_state(eStateMelee);
		else	
			select_state(eStateRun);
	} else if (prev_substate == eStateMelee){
		if (get_state(eStateMelee)->check_completion()) 
			select_state(eStateRun);
		else	
			select_state(eStateMelee);
	} else {
		VERIFY(false);
	}

	get_state_current()->execute();
	
	prev_substate = current_substate;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateCatAttackRatAbstract
