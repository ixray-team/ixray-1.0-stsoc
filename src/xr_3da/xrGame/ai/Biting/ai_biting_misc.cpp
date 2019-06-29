////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Входные воздействия 
// Зрение, слух, вероятность победы, выгодность противника
void CAI_Biting::vfUpdateParameters()
{
	// Обновить память
	EnemyMemory.update			();
	SoundMemory.UpdateHearing	();	
	CorpseMemory.update			();
	HitMemory.update			();
	
	// обновить менеджеры врагов и трупов
	EnemyMan.update				();	
	CorpseMan.update			();
		
	
	hear_dangerous_sound = hear_interesting_sound = false;
	SoundElem se;
	
	if (SoundMemory.IsRememberSound()) {
		SoundMemory.GetSound(se,hear_dangerous_sound);
		hear_interesting_sound = !hear_dangerous_sound;
	}

	// Setup is own additional flags
	m_bDamaged = ((GetHealth() < get_sd()->m_fDamagedThreshold) ? true : false);
}


