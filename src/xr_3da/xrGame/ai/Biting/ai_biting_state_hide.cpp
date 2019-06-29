#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingHide::CBitingHide(CAI_Biting *p)
{
	pMonster = p;
}

void CBitingHide::Init()
{
	inherited::Init();

}

void CBitingHide::Reset()
{
	inherited::Reset();
}

void CBitingHide::Run()
{
	LOG_EX("Hide state...");
	pMonster->MotionMan.m_tAction = ACT_SLEEP;
}

