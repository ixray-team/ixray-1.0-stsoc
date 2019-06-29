#include "stdafx.h"
#include "pseudodog.h"
#include "pseudodog_state_growling.h"

#include "../../../WeaponMagazined.h"
#include "../../../inventory.h"
#include "../../../actor.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"

CPseudodogGrowling::CPseudodogGrowling(CAI_PseudoDog *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CPseudodogGrowling::Init()
{
	LOG_EX("GROWLING:: Init");
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();

	VERIFY(enemy);

	time_next_psi_attack	= 0;

}


void CPseudodogGrowling::Run()
{
	// ≈сли враг изменилс€, инициализировать состо€ние
	if (pMonster->EnemyMan.get_enemy() != enemy) Init();
	
	// —мотреть на врага 
	pMonster->DirMan.face_target(pMonster->EnemyMan.get_enemy_position());

	m_tAction = ACTION_THREATEN;
	if (CheckPsiAttack()) m_tAction = ACTION_PSI_ATTACK; 
	if (pMonster->EnemyMan.get_enemy_time_last_seen() + 5000 < m_dwCurrentTime) m_tAction = ACTION_ANGRY_IDLE;


	switch (m_tAction) {
	/********************/
	case ACTION_THREATEN: 
	/********************/
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;	
		pMonster->MotionMan.SetSpecParams(ASP_THREATEN);
		pMonster->sound().play(MonsterSpace::eMonsterSoundThreaten, 0,0, pMonster->get_sd()->m_dwAttackSndDelay);
		break;
	/*************************/
	case ACTION_PSI_ATTACK:
	/*************************/
		pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;	
		pMonster->MotionMan.SetSpecParams(ASP_PSI_ATTACK);
		pMonster->sound().play(MonsterSpace::eMonsterSoundPsyAttack);
		//pMonster->play_effect_sound();

		time_next_psi_attack			= m_dwCurrentTime + Random.randI(2000,4000);
		break;
	/**********************/
	case ACTION_ANGRY_IDLE:
	/**********************/
		pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;	
		pMonster->sound().play(MonsterSpace::eMonsterSoundThreaten, 0,0, pMonster->get_sd()->m_dwAttackSndDelay);
		break;
	}
	
}

bool CPseudodogGrowling::CheckPsiAttack()
{
	if (!pMonster->ability_psi_attack()) return false;
	if (pMonster->EnemyMan.get_enemy_time_last_seen() != m_dwCurrentTime) return false;

	if (time_next_psi_attack > m_dwCurrentTime) return false;

	// проверить направленность на цель
	float h,p;
	Fvector().sub(pMonster->EnemyMan.get_enemy_position(), pMonster->Position()).getHP(h,p);
	float my_h,my_p;
	pMonster->Direction().getHP(my_h,my_p);

	if (angle_difference(h,my_h) > deg(10) ) return false;

	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(enemy));
	if (!pA) return false;

	if (pMonster != smart_cast<CBaseMonster *>(pA->ObjectWeLookingAt())) return false;

	CWeaponMagazined *pWeapon = smart_cast<CWeaponMagazined *>(pA->m_inventory->ActiveItem());
	if (!pWeapon) return false;

	return true;
}


