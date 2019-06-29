#include "stdafx.h"
#include "burer_state_base.h"
#include "burer.h"

//CBurerStateBase::CBurerStateBase(CBurer *object, CState *rest, CState *attack, CState *panic, CState *eat, CState *snd_danger, CState *snd_interest) 
//{
//	monster = object;
//
//	add_state(eStateRest,					rest);
//	add_state(eStateAttack,					attack);
//	add_state(eStatePanic,					panic);
//	add_state(eStateEat,					eat);
//	add_state(eStateHearDangerousSound,		snd_danger);
//	add_state(eStateHearInterestingSound,	snd_interest);
//}
//
//CBurerStateBase::~CBurerStateBase() 
//{
//	free_mem();
//}
//
//void CBurerStateBase::execute() 
//{ 
//	u32 state_id = u32(-1);
//	
//	if (monster->EnemyMan.get_enemy()) {
//		switch (monster->EnemyMan.get_danger_type()) {
//			case eVeryStrong:					state_id	= eStatePanic; break;
//			case eStrong:		
//			case eNormal:	
//			case eWeak:							state_id	= eStateAttack; break;
//		}
//	} else if (monster->hear_dangerous_sound || monster->hear_interesting_sound) {
//		if (monster->hear_dangerous_sound)		state_id	= eStateHearDangerousSound;
//		if (monster->hear_interesting_sound)	state_id	= eStateHearInterestingSound;	
//	} else	if (monster->CorpseMan.get_corpse() && ((monster->GetSatiety() < monster->_sd->m_fMinSatiety) || monster->flagEatNow))					
//												state_id	= eStateEat;
//	else										state_id	= eStateRest;
//
//	select_state(state_id);
//
//	// выполнить текущее состояние
//	CState *state = get_state(current_substate);
//	state->execute();
//}
//
