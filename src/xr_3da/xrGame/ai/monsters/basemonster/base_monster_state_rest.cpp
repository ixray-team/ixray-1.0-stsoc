#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../ai_monster_movement.h"
#include "../../../entitycondition.h"
#include "../../../level.h"

#define DELTA_NEXT_THINK	5000


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterRest::CBaseMonsterRest(CBaseMonster *p)  
{
	pMonster = p;
	SetPriority(PRIORITY_LOW);
}

void CBaseMonsterRest::Init()
{
	LOG_EX2("%s: rest init", *"*/ pMonster->cName() /*"*);
	IState::Init();

	// если есть путь - дойти до конца (последствия преследования врага) FIXME
	if (pMonster->movement().IsMovingOnPath()) m_bFollowPath = true;
	else m_bFollowPath = false;

	m_dwLastPlanTime	= 0;
	m_tAction			= ACTION_WALK;

	pMonster->movement().initialize_movement();
}

void CBaseMonsterRest::Replanning()
{
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->get_sd()->m_dwDayTimeBegin) && (day_time <= pMonster->get_sd()->m_dwDayTimeEnd)) {  // день?

		bool bNormalSatiety = (pMonster->conditions().GetSatiety() > pMonster->get_sd()->m_fMinSatiety) && (pMonster->conditions().GetSatiety() < pMonster->get_sd()->m_fMaxSatiety); 
		if (bNormalSatiety) {		// отдых
			m_tAction = ACTION_SATIETY_GOOD;
		} else {					// бродит, ищет еду
			m_tAction = ACTION_WALK;
		}
	} else { // ночь

		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // спокоен, спит
			// спать
			m_tAction = ACTION_SLEEP;
		} else {
			// бродить (настороженно), часто осматриваться по сторонам
			m_tAction = ACTION_WALK_CIRCUMSPECTION;
		}
	}
}


void CBaseMonsterRest::Run()
{
	if (m_bFollowPath) 
		if (pMonster->movement().path_completed()) m_bFollowPath = false;

	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_PATH_END;
	} else {
		
		// проверить нужно ли провести перепланировку
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, DELTA_NEXT_THINK);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}

	
	pMonster->MotionMan.accel_activate(eAT_Calm);

	switch (m_tAction) {
		case ACTION_WALK:						// обход точек графа
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			
			// ходить по точкам графа
			pMonster->movement().detour_graph_points();

			break;
		case ACTION_SATIETY_GOOD:				// стоять, ничего не делать
			pMonster->MotionMan.m_tAction = ACT_REST;
			break;
		case ACTION_SLEEP:						// лежать
			pMonster->MotionMan.m_tAction = ACT_SLEEP;
			break;
		case ACTION_WALK_CIRCUMSPECTION:		// повернуться на 90 град.
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

			// ходить по точкам графа
			pMonster->movement().detour_graph_points();
			break;
		case ACTION_WALK_PATH_END:
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			
			break;
	}

	pMonster->set_state_sound	(MonsterSpace::eMonsterSoundIdle);
}

TTime CBaseMonsterRest::UnlockState(TTime cur_time)
{
	return cur_time;
}

