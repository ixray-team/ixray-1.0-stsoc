#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../../../game_graph.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterDetour::CBaseMonsterDetour(CBaseMonster *p)
{
	pMonster = p;
}

void CBaseMonsterDetour::Reset()
{
	inherited::Reset();
}

void CBaseMonsterDetour::Init()
{
	inherited::Init();

}

void CBaseMonsterDetour::Run()
{
	LOG_EX("Detour...");
	pMonster->MotionMan.m_tAction = ACT_SLEEP;
}


