#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBitingPanic::Init()
{
	inherited::Init();

	position					= pMonster->EnemyMan.get_enemy_position();
		
	m_tAction					= ACTION_RUN;
	m_tPrevAction				= m_tAction;

	target_vertex_id			= u32(-1);
	last_time_cover_selected	= 0;
	m_dwFaceEnemyLastTime		= 0;
}

void CBitingPanic::Run()
{
	// если враг очень близко - атаковать
	float m_fDistMin, m_fDistMax, dist;
	dist = pMonster->GetEnemyDistances(m_fDistMin, m_fDistMax);
	if (dist < m_fDistMax) m_tAction = ACTION_ATTACK_MELEE;
	else {
		
		if (m_tPrevAction == ACTION_ATTACK_MELEE) m_tAction = ACTION_RUN;

		if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime) {
			position = pMonster->EnemyMan.get_enemy_position();
			target_vertex_id = u32(-1);
		} 

		if (target_vertex_id == u32(-1) && (last_time_cover_selected + 5000 < m_dwCurrentTime)) {
			if (!pMonster->GetCoverFromEnemy(position, target_pos, target_vertex_id)) {
				target_vertex_id			= u32(-1);
				last_time_cover_selected	= m_dwCurrentTime;
			}
		}
	}

	switch (m_tAction) {
		
		/**************/
		case ACTION_RUN:
		/**************/
		
			pMonster->MotionMan.m_tAction	= ACT_RUN;

			if (target_vertex_id != u32(-1)) {
				pMonster->MoveToTarget			(target_pos, target_vertex_id);
			} else {
				pMonster->MoveAwayFromTarget	(position);
			}
			
			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);
			
			if (pMonster->EnemyMan.get_enemy_time_last_seen() + 10000 < m_dwCurrentTime) m_tAction = ACTION_FACE_BACK_SCARED;
			if (pMonster->IsPathEnd(2.5f)) target_vertex_id = u32(-1);
			
			break;
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			pMonster->FaceTarget(position);

			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(10)) {
				if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime) m_tAction = ACTION_RUN;
			}

			break;
		
		/*************************/
		case ACTION_ATTACK_MELEE:
		/*************************/
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 1200);
				pMonster->FaceTarget(pMonster->EnemyMan.get_enemy());
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			break;
	}
	
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

	m_tPrevAction = m_tAction;
}

bool CBitingPanic::CheckCompletion()
{
	return false;
}

