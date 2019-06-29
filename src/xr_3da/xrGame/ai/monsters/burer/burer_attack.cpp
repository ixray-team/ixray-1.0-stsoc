#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer_attack_tele.h"
#include "burer_attack_melee.h"
#include "burer_attack_gravi.h"
#include "burer_attack_misc.h"
#include "burer_attack.h"
#include "burer.h"


#define GRAVI_PERCENT		50
#define TELE_PERCENT		50
#define RUN_AROUND_PERCENT	20

CBurerAttack::CBurerAttack(CBurer *p)  
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);

	stateTele		= xr_new<CBurerAttackTele>(p);
	stateMelee		= xr_new<CBurerAttackMelee>(p);
	stateGravi		= xr_new<CBurerAttackGravi>(p);
	stateRunAround	= xr_new<CBurerAttackRunAround>(p);
	stateFaceEnemy	= xr_new<CBurerAttackFaceTarget>(p);
	cur_state	= 0;
}

CBurerAttack::~CBurerAttack() 
{
	xr_delete(stateTele);
	xr_delete(stateMelee);
	xr_delete(stateGravi);
	xr_delete(stateRunAround);
	xr_delete(stateFaceEnemy);

}


void CBurerAttack::Init()
{
	LOG_EX("attack init");
	IState::Init();

	// Получить врага
	enemy			= pMonster->EnemyMan.get_enemy();
	
	stateTele->UpdateExternal();
	stateMelee->UpdateExternal();
	stateGravi->UpdateExternal();
	stateRunAround->UpdateExternal();
	stateFaceEnemy->UpdateExternal();

	b_need_reselect = true;
	prev_state		= 0;
}



void CBurerAttack::Run()
{
	// Если враг изменился, инициализировать состояние
	if (pMonster->EnemyMan.get_enemy() != enemy) {
		Init();
		if (cur_state) {
			cur_state->CriticalInterrupt();
			cur_state->Reset();
		}
		b_need_reselect = true;		
	}

	if (cur_state && (cur_state != stateMelee)) {
		if (stateMelee->CheckStartCondition()) {
			cur_state->CriticalInterrupt();
			cur_state->Reset();
			b_need_reselect = true;		
		}
	}

	// проверить нужно ли перевыбирать состояние
	if (b_need_reselect) {
		ReselectState();
		b_need_reselect = false;
	}

	// выполнить текущее
	cur_state->Execute(m_dwCurrentTime);

	// если текущее выполнено перевыбрать на след update
	if (cur_state->IsCompleted()) {
		
		// сохранить предыдущее состояние
		prev_state = cur_state;

		cur_state->Done();
		cur_state = 0;
		b_need_reselect = true;
	}
}


void CBurerAttack::Done()
{
	inherited::Done();
	
	pMonster->StopGraviPrepare();
	pMonster->MotionMan.TA_Deactivate();
	pMonster->CTelekinesis::Deactivate();
	
	if (cur_state) {
		cur_state->Done();
		cur_state->Reset();
	}
}

void CBurerAttack::select_state(IState *state)
{
	if (cur_state != state) {
		
		if (cur_state) {
			if (cur_state->IsCompleted()) cur_state->Done();
			else cur_state->CriticalInterrupt();
		
			cur_state->Reset();
		}

		cur_state = state;
		cur_state->Activate();
	}
}

void CBurerAttack::ReselectState()
{
	if (stateMelee->CheckStartCondition()) {
		select_state(stateMelee);
		return;
	}

	bool enable_gravi	= stateGravi->CheckStartCondition();
	bool enable_tele	= stateTele->CheckStartCondition();

	if (!enable_gravi && !enable_tele) {
		if (prev_state == stateRunAround) 
			select_state(stateFaceEnemy);
		else 	
			select_state(stateRunAround);
		return;
	}

	if (enable_gravi && enable_tele) {
	
		u32 rnd_val = ::Random.randI(GRAVI_PERCENT + TELE_PERCENT + RUN_AROUND_PERCENT);
		u32 cur_val = GRAVI_PERCENT;
		
		if (rnd_val < cur_val) {
			select_state(stateGravi);
			return;
		}
		
		cur_val += TELE_PERCENT;
		if (rnd_val < cur_val) {
			select_state(stateTele);
			return;
		}

		select_state(stateRunAround);
		return;
	}

	if ((prev_state == stateRunAround) || (prev_state == stateFaceEnemy)) {
		if (enable_gravi) select_state(stateGravi);
		else select_state(stateTele);
	} else {
		select_state(stateRunAround);
	}
}
