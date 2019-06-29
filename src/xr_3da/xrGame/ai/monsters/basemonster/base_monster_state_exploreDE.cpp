#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterExploreDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterExploreDE::CBaseMonsterExploreDE(CBaseMonster *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBaseMonsterExploreDE::Init()
{
	inherited::Init();
	R_ASSERT(pMonster->SoundMemory.IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->SoundMemory.GetSound(se,bDangerous);	// возвращает самый опасный звук
}

void CBaseMonsterExploreDE::Run()
{
//	SoundElem	se;
//	bool		bTemp;
//	pMonster->GetSound(se, bTemp);
//	if (m_tSound.time + 2000 < se.time) Init();

	SoundElem se;
	bool bDangerous;
	pMonster->SoundMemory.GetSound(se,bDangerous);	// возвращает самый опасный звук

	switch(m_tAction) {
	case ACTION_LOOK_AROUND:
		// look round here
		break;
	case ACTION_HIDE:
		//pMonster->vfChoosePointAndBuildPath(pMonster->m_tSelectorCover, 0, true, 0,2000);

		// Установить параметры движения
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		break;
	}
}

bool CBaseMonsterExploreDE::CheckCompletion()
{	
	return false;
}
