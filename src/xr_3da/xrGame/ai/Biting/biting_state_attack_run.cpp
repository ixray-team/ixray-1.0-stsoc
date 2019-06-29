
#include "stdafx.h"
#include "biting_state_attack_run.h"
#include "ai_biting.h"

CStateBitingAttackRun::CStateBitingAttackRun(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingAttackRun::~CStateBitingAttackRun()
{
}

void CStateBitingAttackRun::initialize()
{
	inherited::initialize();
}

void CStateBitingAttackRun::execute()
{
	m_object->MotionMan.m_tAction	= ACT_RUN;
	m_object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundAttack, 0,0,m_object->_sd->m_dwAttackSndDelay);
	m_object->MoveToTarget			(m_object->m_tEnemy.obj);
}

void CStateBitingAttackRun::finalize()
{
	inherited::finalize();
}


