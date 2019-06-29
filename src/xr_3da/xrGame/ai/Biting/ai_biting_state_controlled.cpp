#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "../controlled_entity.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#include "../ai_monster_utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingControlled class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingControlled::CBitingControlled(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBitingControlled::Init()
{
	inherited::Init ();

	enemy				= 0;
	m_tAction			= m_tPrevAction = ACTION_RUN;
	LastTimeRebuild		= 0;
	FaceEnemyLastTime	= 0;
}

void CBitingControlled::Run()
{
	CControlledEntityBase *entity = smart_cast<CControlledEntityBase *>(pMonster);
	VERIFY(entity);

	switch (entity->get_data().m_task) {
		case eTaskFollow:
			
			enemy = entity->get_data().m_object;
			ExecuteFollow();

			break;
		case eTaskAttack:
			
			enemy = entity->get_data().m_object;
			ExecuteAttack();
			
			break;
		default:

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	} 
}

void CBitingControlled::ExecuteAttack()
{
	// -----------------------------------------------------------------------
	// обновить минимальную и максимальную дистанции до врага
	float m_fDistMin, m_fDistMax;
	float dist = pMonster->GetEnemyDistances(m_fDistMin, m_fDistMax, enemy);

	// определить переменные машины состояний

	if ((m_tPrevAction == ACTION_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_MELEE);
	
	bool	bNeedRebuild = false;
	bool	squad_active = false;
	CMonsterSquad	*pSquad = 0;

	switch (m_tAction)	{
		// ************
		case ACTION_RUN: 		 // бежать на врага
		// ************	
			pMonster->MotionMan.m_tAction	= ACT_RUN;
			pMonster->CMonsterMovement::set_try_min_time(false);

			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			DO_IN_TIME_INTERVAL_BEGIN(LastTimeRebuild,100 + 50.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			if (IS_NEED_REBUILD()) bNeedRebuild = true;

			pSquad = monster_squad().get_squad(pMonster);
			squad_active = pSquad && pSquad->SquadActive();

			if (bNeedRebuild) {
				// Получить позицию, определённую груп. интелл.
				if (squad_active) {
					//TTime			squad_ai_last_updated;
					//Fvector			target = pSquad->GetTargetPoint(pMonster, squad_ai_last_updated);
					//pMonster->set_dest_direction	(target);
				}
				pMonster->MoveToTarget(enemy);
			}

			if (squad_active)
				pMonster->set_use_dest_orient	(true);

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;
			
		// *********************
		case ACTION_MELEE:		// атаковать вплотную
		// *********************
			
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(FaceEnemyLastTime, 1200);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			break;
	}
	
	m_tPrevAction = m_tAction;
}

void CBitingControlled::ExecuteFollow()
{
	float dist = pMonster->Position().distance_to(enemy->Position());	
	
	if ((pMonster->MotionMan.m_tAction == ACT_STAND_IDLE) && (dist < 15.f)) 
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	else 
		pMonster->MotionMan.m_tAction = ((dist > 10.0f) ? ACT_WALK_FWD : ACT_STAND_IDLE);

	if (pMonster->MotionMan.m_tAction == ACT_WALK_FWD){
		pMonster->MoveToTarget(random_position(enemy->Position(), 10.f));
	}

}

