#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../ai_monster_movement.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterExploreDNE::CBaseMonsterExploreDNE(CBaseMonster *p)
{
	pMonster = p;
	
	SetPriority(PRIORITY_NORMAL);
}


void CBaseMonsterExploreDNE::Init()
{
	IState::Init();	
	bool bTemp;
	pMonster->SoundMemory.GetSound(m_tSound, bTemp);

	flag_once_1				= false;
	SavedPosition			= pMonster->Position();
	StartPosition			= SavedPosition;			// сохранить стартовую позицию
	LastPosition.set		(0.f,0.f,0.f);			
	m_fRunAwayDist			= 50.f;
	m_dwLastPosSavedTime	= 0;
	m_dwStayLastTimeCheck	= m_dwCurrentTime;

	m_tAction				= ACTION_RUN_AWAY;
	
	pMonster->movement().initialize_movement	();
}

void CBaseMonsterExploreDNE::Run()
{
	// если новый звук, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->SoundMemory.GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	switch (m_tAction) {
	case ACTION_RUN_AWAY: // убегать на N метров от звука
		
		pMonster->MotionMan.m_tAction						= ACT_RUN;
		pMonster->movement().set_retreat_from_point	(m_tSound.position);
		pMonster->movement().set_generic_parameters	();

		break;
	case ACTION_LOOK_BACK_POSITION:			// повернуться в сторону звука
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

		DO_ONCE_BEGIN(flag_once_1);
			pMonster->LookPosition(SavedPosition);
		DO_ONCE_END();

		// если уже повернулся, перейти в след. состояние
		if (angle_difference(pMonster->movement().m_body.current.yaw, pMonster->movement().m_body.target.yaw) <= PI_DIV_6/6) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		pMonster->MotionMan.m_tAction =	 ACT_LOOK_AROUND;
		break;
	}
	
	pMonster->set_state_sound(MonsterSpace::eMonsterSoundIdle);
}
