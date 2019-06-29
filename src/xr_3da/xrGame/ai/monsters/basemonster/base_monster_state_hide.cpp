#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterHide::CBaseMonsterHide(CBaseMonster *p)
{
	pMonster = p;
}

void CBaseMonsterHide::Init()
{
	inherited::Init();

}

void CBaseMonsterHide::Reset()
{
	inherited::Reset();
}

void CBaseMonsterHide::Run()
{
	LOG_EX("Hide state...");
	pMonster->MotionMan.m_tAction = ACT_SLEEP;
}

