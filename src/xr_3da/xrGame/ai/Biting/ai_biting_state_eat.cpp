#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"

#include "../../phmovementcontrol.h"
#include "../../PhysicsShell.h"
#include "../../phcapture.h"

#define		REST_AFTER_LUNCH_TIME			5000
#define		DIST_SLOW_APPROACH_TO_CORPSE	5.0f

//#define		EAT_WITHOUT_DRAG

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingEat::CBitingEat(CAI_Biting *p)  
{
	pMonster	= p;

	SetPriority(PRIORITY_LOW);
}

void CBitingEat::Reset()
{
	IState::Reset();

	pCorpse				= 0;

	m_dwLastTimeEat		= 0;
	m_dwEatInterval		= 1000;
}
void CBitingEat::Init()
{
	LOG_EX			("eat init");
	IState::Init	();

	// Получить инфо о трупе
	const CEntityAlive *p_corpse = pMonster->CorpseMan.get_corpse();
	VERIFY			(p_corpse);
	pCorpse			= const_cast<CEntityAlive *>(p_corpse);

	bEatRat			= (smart_cast<CAI_Rat *>(pCorpse) ? true : false);
	m_fDistToCorpse = ((bEatRat)? 1.0f : pMonster->get_sd()->m_fDistToCorpse); 

	SavedPos			= pCorpse->Position();		// сохранить позицию трупа
	m_fDistToDrag		= 20.f;
	bDragging			= false;

	m_tAction			= ACTION_CORPSE_APPROACH_RUN;

	flag_once_1			= false;
	m_dwStandStart		= 0;

	m_dwPrepareDrag		= 0;
	bEating				= false;
	
	bRestAfterLunch		= false;
	bHideAfterLunch		= false;
	
	m_dwTimeStartRest	= 0;

	pMonster->flagEatNow = true;
	flag_once_1			= false;

	rebuild_path		= 0;

	Captured			= false;
	m_dwLastImpulse		= 0;

	cover_position		= Fvector().set(0.f,0.f,0.f);		
	cover_vertex_id		= u32(-1);
}

void CBitingEat::Run()
{
	// Если новый труп, снова инициализировать состояние 
	const CEntityAlive *cur_corpse = pMonster->CorpseMan.get_corpse();
	if (!cur_corpse || !pCorpse->m_pPhysicsShell) {Done(); return;}
	if (pCorpse != const_cast<CEntityAlive *>(cur_corpse)) Init();

	// Определить позицию ближайшей боны у трупа
	Fvector nearest_bone_pos;
	if ((pCorpse->m_pPhysicsShell == NULL) || (!pCorpse->m_pPhysicsShell->bActive)) {
		nearest_bone_pos	= pCorpse->Position(); 
	} else nearest_bone_pos = pMonster->m_PhysicMovementControl->PHCaptureGetNearestElemPos(pCorpse);
	
	float cur_dist = nearest_bone_pos.distance_to(pMonster->Position());

	if (bHideAfterLunch) m_tAction = ACTION_GET_HIDE;
	else if (bRestAfterLunch) m_tAction = ACTION_LITTLE_REST;
	else if (bEating && (cur_dist > m_fDistToCorpse)) m_tAction = ACTION_WALK;

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // расстояние, на которое уже оттащен труп

	Fvector		approach_pos;
	u32			approach_vertex_id;
	
	bool bNeedRebuild = false; 

	switch (m_tAction) {
	/**********************************/
	case ACTION_CORPSE_APPROACH_RUN:	// бежать к трупу
	/**********************************/
		pMonster->MotionMan.m_tAction = ACT_RUN;
		pMonster->MotionMan.accel_activate		(eAT_Calm);

		approach_vertex_id = ai().level_graph().check_position_in_direction(pCorpse->level_vertex_id(), pCorpse->Position(), nearest_bone_pos);
		if (approach_vertex_id == -1) {
			approach_vertex_id	= pCorpse->level_vertex_id();
			approach_pos		= ai().level_graph().vertex_position(approach_vertex_id);
		} else {
			approach_pos = nearest_bone_pos;
		}
		R_ASSERT(ai().level_graph().inside(approach_vertex_id,approach_pos));
	
		if (IS_NEED_REBUILD()) bNeedRebuild = true;
		if (pMonster->CMovementManager::level_dest_vertex_id() != approach_vertex_id) bNeedRebuild = true;
		
		if (bNeedRebuild) pMonster->MoveToTarget(approach_pos,approach_vertex_id);

		if (cur_dist < DIST_SLOW_APPROACH_TO_CORPSE) m_tAction = ACTION_CORPSE_APPROACH_WALK;

		break;
	/**********************************/
	case ACTION_CORPSE_APPROACH_WALK:
	/**********************************/
		pMonster->MotionMan.accel_activate		(eAT_Calm);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		pMonster->MoveToTarget(nearest_bone_pos,pCorpse->level_vertex_id());
		
		if (cur_dist < m_fDistToCorpse) {
			
			// если монстр подбежал к трупу, необходимо отыграть проверку трупа
			pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
			
#ifndef		EAT_WITHOUT_DRAG
			m_tAction = ( (pMonster->ability_can_drag()) ?  ACTION_PREPARE_DRAG : ACTION_EAT );
#else	
			m_tAction = ACTION_EAT;
#endif
			m_dwPrepareDrag	= m_dwCurrentTime;

		}

		break;
	/****************/
	case ACTION_EAT:
	/****************/

		pMonster->MotionMan.m_tAction	= ACT_EAT;

		bEating = true;
		if (pMonster->GetSatiety() >= 0.9f) bHideAfterLunch = true;

		// съесть часть
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, u32(1000/pMonster->get_sd()->m_fEatFreq));
			pMonster->ChangeSatiety(pMonster->get_sd()->m_fEatSlice);
			pCorpse->m_fFood -= pMonster->get_sd()->m_fEatSliceWeight;
		DO_IN_TIME_INTERVAL_END();
	

		// Реализация 'Трясти труп во время еды'

		break;
	/**********************************/
	case ACTION_GET_HIDE: 
	/**********************************/
		
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;		

		pMonster->MoveAwayFromTarget	(pCorpse->Position());

		if (cur_dist > 10.f || (IS_NEED_REBUILD() && (cur_dist > 3.f))) {
			m_tAction = ACTION_LITTLE_REST;
			bHideAfterLunch = false;
			bRestAfterLunch	= true;
			m_dwTimeStartRest = m_dwCurrentTime;
		}
		break;
	/*************************/
	case ACTION_LITTLE_REST:
	/*************************/
		pMonster->MotionMan.m_tAction = ACT_REST; 
		
		if (m_dwTimeStartRest + REST_AFTER_LUNCH_TIME < m_dwCurrentTime) {
			pMonster->flagEatNow	= false;
			bRestAfterLunch			= false; 
			//pMonster->AddIgnoreObject(pCorpse);
		}

		break;

	/********************/
	case ACTION_WALK:
	/*******************/

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		pMonster->MoveToTarget(nearest_bone_pos,pCorpse->level_vertex_id());

		if (cur_dist < m_fDistToCorpse) {
			m_tAction = ACTION_EAT;
		}
		break;
	
	/*************************/
	case ACTION_PREPARE_DRAG:
	/*************************/

		if (m_dwPrepareDrag + 1000 < m_dwCurrentTime) {
			// Если труп крысы || если не получилось взять
			bDragging = false;
			m_tAction = ACTION_EAT;

			if (!bEatRat && pMonster->ability_can_drag()) {	// Если не труп крысы и может тащить
				// пытаться взять труп
				pMonster->m_PhysicMovementControl->PHCaptureObject(pCorpse);

				if (!pMonster->m_PhysicMovementControl->PHCapture()->Failed()) {
					// тащить труп
					bDragging		= true;
					m_tAction		= ACTION_DRAG;

					if (!pMonster->GetCorpseCover(cover_position, cover_vertex_id)) {
						cover_vertex_id = u32(-1);
					}					
				} 
			} 
		}

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		break;

	/****************/
	case ACTION_DRAG:
	/****************/

		// Установить параметры движения
		pMonster->MotionMan.m_tAction = ACT_DRAG; 
		pMonster->MotionMan.SetSpecParams(ASP_MOVE_BKWD);
		
		if (cover_vertex_id != u32(-1)) {
			pMonster->MoveToTarget(cover_position, cover_vertex_id);
		} else {
			pMonster->MoveAwayFromTarget	(pCorpse->Position());
		}
		
		pMonster->MotionMan.accel_activate(eAT_Calm);

		// если не может тащить
		if (0 == pMonster->m_PhysicMovementControl->PHCapture()) m_tAction = ACTION_EAT; 
		
		if ((cover_vertex_id != u32(-1) && (pMonster->Position().distance_to(cover_position) < 2.f)) || 
			((cover_vertex_id == u32(-1)) && (saved_dist > m_fDistToDrag))) {
			
			// бросить труп
			pMonster->m_PhysicMovementControl->PHReleaseObject();

			bDragging = false; 
			m_tAction = ACTION_EAT;
		}
		break;
	}


	(pMonster->MotionMan.m_tAction == ACT_EAT) ?
		pMonster->State_PlaySound(MonsterSpace::eMonsterSoundEat, pMonster->get_sd()->m_dwEatSndDelay)	:
		pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->get_sd()->m_dwIdleSndDelay) ;		

}

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow	= false;

	// если тащит труп - бросить
	if (bDragging) {
		pMonster->m_PhysicMovementControl->PHReleaseObject();
	}
}

bool CBitingEat::CheckCompletion()
{

	return false;
}

