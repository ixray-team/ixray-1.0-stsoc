#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "../../game_graph.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingDetour::CBitingDetour(CAI_Biting *p)
{
	pMonster = p;
}

void CBitingDetour::Reset()
{
	inherited::Reset();
}

void CBitingDetour::Init()
{
	inherited::Init();

}

void CBitingDetour::Run()
{
	LOG_EX("Detour...");
	pMonster->MotionMan.m_tAction = ACT_SLEEP;
}


