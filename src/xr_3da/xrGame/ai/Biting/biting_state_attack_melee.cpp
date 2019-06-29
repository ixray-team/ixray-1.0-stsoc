
#include "stdafx.h"
#include "biting_state_attack_melee.h"
#include "ai_biting.h"

CStateBitingAttackMelee::CStateBitingAttackMelee(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingAttackMelee::~CStateBitingAttackMelee()
{
}

void CStateBitingAttackMelee::initialize()
{
	inherited::initialize();
}

void CStateBitingAttackMelee::execute()
{
	const CEntity *enemy = m_object->m_tEnemy.obj;

	m_object->MotionMan.m_tAction	= ACT_ATTACK;
	m_object->FaceTarget			(enemy);
	m_object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundAttack, 0,0,m_object->_sd->m_dwAttackSndDelay);
}	

void CStateBitingAttackMelee::finalize()
{
	inherited::finalize();
}
