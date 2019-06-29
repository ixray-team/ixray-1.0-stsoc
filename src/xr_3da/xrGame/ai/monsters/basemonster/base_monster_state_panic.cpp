#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../ai_monster_movement.h"
#include "../../../sound_player.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBaseMonsterPanic::CBaseMonsterPanic(CBaseMonster *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBaseMonsterPanic::Init()
{
	inherited::Init();

	position					= pMonster->EnemyMan.get_enemy_position();
		
	m_tAction					= ACTION_RUN;
	m_tPrevAction				= m_tAction;

	target_vertex_id			= u32(-1);
	last_time_cover_selected	= 0;

	pMonster->movement().initialize_movement	();
}

void CBaseMonsterPanic::Run()
{
	// если враг очень близко - атаковать
//	float m_fDistMin = pMonster->MeleeChecker.get_min_distance	();
	float m_fDistMax = pMonster->MeleeChecker.get_max_distance	();
	float dist = pMonster->MeleeChecker.distance_to_enemy		(pMonster->EnemyMan.get_enemy());


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
				pMonster->movement().set_target_point		(target_pos, target_vertex_id);
			} else {
				pMonster->movement().set_retreat_from_point	(position);
			}
			
			pMonster->movement().set_generic_parameters	();

			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);
			
			if (pMonster->EnemyMan.get_enemy_time_last_seen() + 10000 < m_dwCurrentTime) m_tAction = ACTION_FACE_BACK_SCARED;
			if (pMonster->movement().IsPathEnd(2.5f)) target_vertex_id = u32(-1);
			
			break;
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			pMonster->DirMan.face_target(position);

			if (angle_difference(pMonster->movement().m_body.current.yaw, pMonster->movement().m_body.target.yaw) < deg(10)) {
				if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime) m_tAction = ACTION_RUN;
			}

			break;
		
		/*************************/
		case ACTION_ATTACK_MELEE:
		/*************************/
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// Смотреть на врага 
			pMonster->DirMan.face_target(pMonster->EnemyMan.get_enemy(), 1200);

			pMonster->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			break;
	}
	
	pMonster->sound().play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

	m_tPrevAction = m_tAction;
}

bool CBaseMonsterPanic::CheckCompletion()
{
	return false;
}

