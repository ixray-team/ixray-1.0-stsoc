#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer.h"
#include "burer_scan.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"

void CBurerScan::Run()
{
	pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
	pMonster->sound().play(MonsterSpace::eMonsterSoundIdle, 0,0,pMonster->get_sd()->m_dwIdleSndDelay);
}
