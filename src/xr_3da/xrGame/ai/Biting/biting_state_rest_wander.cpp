
#include "stdafx.h"
#include "biting_state_rest_wander.h"
#include "ai_biting.h"

CStateBitingWander::CStateBitingWander	(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingWander::~CStateBitingWander	()
{
}

void CStateBitingWander::initialize()
{
	inherited::initialize();
}

void CStateBitingWander::execute()
{
	m_object->MotionMan.m_tAction	= ACT_WALK_FWD;
	m_object->WalkNextGraphPoint	();
	m_object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundIdle, 0,0,m_object->_sd->m_dwIdleSndDelay);
}

void CStateBitingWander::finalize()
{
	inherited::finalize();
}


