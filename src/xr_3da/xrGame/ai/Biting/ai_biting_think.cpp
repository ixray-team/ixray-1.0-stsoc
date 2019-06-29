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

	// Инициализировать
	InitThink								();
	MotionMan.ScheduledInit					();
	CMonsterMovement::Update_Initialize		();

	// Обновить память
	START_PROFILE("AI/Base Monster/Think/Update Memory");
	vfUpdateParameters						();
	STOP_PROFILE;

	// Обновить сквад
	START_PROFILE("AI/Base Monster/Think/Update Squad");
	monster_squad().update					(this);
	STOP_PROFILE;

	// Запустить FSM
	START_PROFILE("AI/Base Monster/Think/FSM");
	if (MotionMan.IsCriticalAction()) disable_path();
	else {
		if (!UpdateStateManager()) {
			StateSelector				();
			CurrentState->Execute		(m_current_update);
			squad_notify				();
		}

		TranslateActionToPathParams		();
	}
	STOP_PROFILE;

	// Построить путь
	START_PROFILE("AI/Base Monster/Think/Build Path");
	CMonsterMovement::Update_Execute		();
	STOP_PROFILE;

	// Обновить анимации
	MotionMan.UpdateScheduled				();

	// установить текущую скорость
	CMonsterMovement::Update_Finalize		();

	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

	STOP_PROFILE;
}

void CAI_Biting::squad_notify()
{
	CMonsterSquad	*squad = monster_squad().get_squad(this);
	SMemberGoal		goal;

	if (CurrentState == stateAttack) {
		goal.type	= MG_AttackEnemy;
		goal.entity	= const_cast<CEntityAlive*>(EnemyMan.get_enemy());
	}  
	
	squad->UpdateGoal(this, goal);
}

