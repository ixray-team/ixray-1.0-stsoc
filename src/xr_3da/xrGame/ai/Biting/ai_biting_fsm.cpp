////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../ai_monster_debug.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../../profiler.h"


void CAI_Biting::Think()
{
	START_PROFILE("AI/Base Monster/Think");

	if (!g_Alive())		return;
	if (getDestroy())	return;

	m_current_update						= Level().timeServer();

	InitThink								();
	MotionMan.ScheduledInit					();

	MotionStats->update						();
	
	START_PROFILE("AI/Base Monster/Think/Update Memory");
	vfUpdateParameters						();
	STOP_PROFILE;

	CMonsterMovement::Frame_Init();

	
	START_PROFILE("AI/Base Monster/Think/Update Squad");
	monster_squad().update(this);
	STOP_PROFILE;
	
	MotionMan.accel_deactivate			();
	
	START_PROFILE("AI/Base Monster/Think/FSM");
	if (!MotionMan.Seq_Active() && !MotionMan.JumpActive()) {
		if (!UpdateStateManager()) {
			StateSelector					();
			CurrentState->Execute			(m_current_update);
			squad_notify();
		}
	} else {
		disable_path();
	}
	STOP_PROFILE;

	


	TranslateActionToPathParams				();

	START_PROFILE("AI/Base Monster/Think/Build Path");
	CMonsterMovement::Frame_Update			();
	STOP_PROFILE;
	
	MotionMan.UpdateScheduled				();

	// установить текущую скорость
	CMonsterMovement::Frame_Finalize		();
	
	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

	STOP_PROFILE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// проверка инерций
		if (!bSkipInertiaCheck)
			if (CurrentState->IsInertia()) {
				if (CurrentState->GetPriority() >= pS->GetPriority()) return;
			}

		CurrentState->Done();
		CurrentState->Reset();
		CurrentState = pS;
		CurrentState->Activate();
	}
}


void CAI_Biting::State_PlaySound(u32 internal_type, u32 max_stop_time)
{
	if (m_bAngry && ((internal_type == eMonsterSoundIdle) ||  (internal_type == eMonsterSoundEat)))
		CSoundPlayer::play(MonsterSpace::eMonsterSoundGrowling, 0, 0, get_sd()->m_dwAttackSndDelay);
	else 
		CSoundPlayer::play(internal_type, 0, 0, max_stop_time);
}

void CAI_Biting::squad_notify()
{
	CMonsterSquad	*squad = monster_squad().get_squad(this);
	SMemberGoal		goal;

	// для определения глобального состояние можно использовать current_substate
	// для определения конкретного типа состояния можно использовать get_state_type()

	if (CurrentState == stateAttack) {

		goal.type	= MG_AttackEnemy;
		goal.entity	= const_cast<CEntityAlive*>(EnemyMan.get_enemy());

	}  

	squad->UpdateGoal(this, goal);
}
