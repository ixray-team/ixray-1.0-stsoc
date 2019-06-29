
#include "stdafx.h"
#include "biting_state_panic_run.h"
#include "ai_biting.h"

CStateBitingPanicRun::CStateBitingPanicRun(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingPanicRun::~CStateBitingPanicRun()
{
}

void CStateBitingPanicRun::initialize()
{
	inherited::initialize();
}

void CStateBitingPanicRun::execute()
{
	m_object->MotionMan.m_tAction	= ACT_RUN;
	m_object->MoveAwayFromTarget	(m_object->m_tEnemy.position);	
	m_object->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,m_object->_sd->m_dwAttackSndDelay);
}

void CStateBitingPanicRun::finalize()
{
	inherited::finalize();
}




