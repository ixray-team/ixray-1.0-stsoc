
#include "stdafx.h"
#include "biting_state_eat_eat.h"
#include "ai_biting.h"

CStateBitingEatEat::CStateBitingEatEat(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatEat::~CStateBitingEatEat()
{
}


void CStateBitingEatEat::initialize()
{
	inherited::initialize();
}

void CStateBitingEatEat::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingEatEat::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatEat::completed() const
{
	return false;
}