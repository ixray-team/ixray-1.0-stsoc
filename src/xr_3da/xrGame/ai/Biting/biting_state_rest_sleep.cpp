
#include "stdafx.h"
#include "biting_state_rest_sleep.h"
#include "ai_biting.h"

CStateBitingSleep::CStateBitingSleep(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingSleep::~CStateBitingSleep()
{
}

void CStateBitingSleep::initialize()
{
	inherited::initialize();
}

void CStateBitingSleep::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingSleep::finalize()
{
	inherited::finalize();
}
