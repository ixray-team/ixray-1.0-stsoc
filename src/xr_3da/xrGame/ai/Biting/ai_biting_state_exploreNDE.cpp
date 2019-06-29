#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

//////////////////////////////////////////////////////////////////////////
// ќписание состо€ни€ <—лышу выгодный и неопасный звук>
// 1. ѕосмотреть на позицию звука
// 2. ѕрийти на точку звука
// 3. ќсмотретьс€
//////////////////////////////////////////////////////////////////////////

#define SF_MANY_DIFFERENT_SOUNDS	(1 << 1)

CBitingExploreNDE::CBitingExploreNDE(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBitingExploreNDE::Init()
{
	LOG_EX("nde init");
	inherited::Init();

	m_tSound	= pMonster->SoundMemory.GetSound();
	target_pos	= m_tSound.position;

	m_tAction = ACTION_LOOK_DESTINATION;
}

void CBitingExploreNDE::Run()
{
	// обновить позицию звука
	SoundElem se = pMonster->SoundMemory.GetSound();
	if ((se.who != m_tSound.who) || (se.position.distance_to(m_tSound.position) > 5.f)) Init();
	else m_tSound = se;


	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// повернутьс€ в сторону звука
		LOG_EX("nde: LOOK DEST");
		
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		pMonster->disable_path();
		pMonster->LookPosition(m_tSound.position);
		
		if (angle_difference(pMonster->CMovementManager::m_body.target.yaw, pMonster->CMovementManager::m_body.current.yaw) < PI_DIV_6) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	
	case ACTION_GOTO_SOUND_SOURCE:			// идти к источнику
		LOG_EX("nde: GOTO_SOUND_SOURCE");
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

		pMonster->MoveToTarget(m_tSound.position);

		// если монстр дошел до позиции звука, перейти к следующему заданию
		if (m_tSound.position.distance_to(pMonster->Position()) < 1.0f) m_tAction = ACTION_LOOK_AROUND; 
		break;
	case ACTION_LOOK_AROUND:
		LOG_EX("nde: LOOK_AROUND");
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		pMonster->disable_path();
		break;
	}

	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->get_sd()->m_dwIdleSndDelay);

}
