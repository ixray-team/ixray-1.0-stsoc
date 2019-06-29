#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDNE::CBitingExploreDNE(CAI_Biting *p)
{
	pMonster = p;
	
	SetPriority(PRIORITY_NORMAL);
}


void CBitingExploreDNE::Init()
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
}

void CBitingExploreDNE::Run()
{
	// если новый звук, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->SoundMemory.GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	switch (m_tAction) {
	case ACTION_RUN_AWAY: // убегать на N метров от звука
		
		pMonster->MotionMan.m_tAction = ACT_RUN;
		pMonster->MoveAwayFromTarget (m_tSound.position);

		// каждую минуту сохранять текущую позицию, а затем развернуться и посмотреть в последнюю позицию
		// т.е. развернуться назад
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPosSavedTime, 1000);
			SavedPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		// проверить условие перехода в след. состояние (достаточное расстояние || стоим на месте около 2 сек)
		if (pMonster->Position().distance_to(StartPosition) > m_fRunAwayDist) m_tAction = ACTION_LOOK_BACK_POSITION;
		
		DO_IN_TIME_INTERVAL_BEGIN(m_dwStayLastTimeCheck, 2000);
			if (pMonster->Position().similar(LastPosition)) m_tAction = ACTION_LOOK_BACK_POSITION;
			LastPosition = pMonster->Position();
		DO_IN_TIME_INTERVAL_END();

		break;
	case ACTION_LOOK_BACK_POSITION:			// повернуться в сторону звука
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

		DO_ONCE_BEGIN(flag_once_1);
			pMonster->LookPosition(SavedPosition);
		DO_ONCE_END();

		// если уже повернулся, перейти в след. состояние
		if (angle_difference(pMonster->CMovementManager::m_body.current.yaw, pMonster->CMovementManager::m_body.target.yaw) <= PI_DIV_6/6) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}
	
	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->get_sd()->m_dwIdleSndDelay);

}
