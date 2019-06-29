#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../ai_monster_movement.h"

//////////////////////////////////////////////////////////////////////////
// ќписание состо€ни€ <—лышу выгодный и неопасный звук>
// 1. ѕосмотреть на позицию звука
// 2. ѕрийти на точку звука
// 3. ќсмотретьс€
//////////////////////////////////////////////////////////////////////////

#define SF_MANY_DIFFERENT_SOUNDS	(1 << 1)

CBaseMonsterExploreNDE::CBaseMonsterExploreNDE(CBaseMonster *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBaseMonsterExploreNDE::Init()
{
	LOG_EX("nde init");
	inherited::Init();

	m_tSound	= pMonster->SoundMemory.GetSound();
	target_pos	= m_tSound.position;

	m_tAction = ACTION_LOOK_DESTINATION;

	pMonster->movement().initialize_movement	();
}

void CBaseMonsterExploreNDE::Run()
{
	// обновить позицию звука
	SoundElem se = pMonster->SoundMemory.GetSound();
	if ((se.who != m_tSound.who) || (se.position.distance_to(m_tSound.position) > 5.f)) Init();
	else m_tSound = se;


	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// повернутьс€ в сторону звука
		LOG_EX("nde: LOOK DEST");
		
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		pMonster->movement().disable_path();
		pMonster->LookPosition(m_tSound.position);
		
		if (angle_difference(pMonster->movement().m_body.target.yaw, pMonster->movement().m_body.current.yaw) < PI_DIV_6) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	
	case ACTION_GOTO_SOUND_SOURCE:			// идти к источнику
		LOG_EX("nde: GOTO_SOUND_SOURCE");
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

		pMonster->movement().set_target_point		(m_tSound.position);
		pMonster->movement().set_generic_parameters	();

		// если монстр дошел до позиции звука, перейти к следующему заданию
		if (m_tSound.position.distance_to(pMonster->Position()) < 1.0f) m_tAction = ACTION_LOOK_AROUND; 
		break;
	case ACTION_LOOK_AROUND:
		LOG_EX("nde: LOOK_AROUND");
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}

	pMonster->set_state_sound	(MonsterSpace::eMonsterSoundIdle);

}
