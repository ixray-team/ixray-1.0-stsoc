////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.cpp
//	Created 	: 27.05.2003
//  Modified 	: 27.05.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "..\\rat\\ai_rat.h"
#include "..\\..\\PhysicsShell.h"
#include "..\\..\\phcapture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingRest::CBitingRest(CAI_Biting *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();			// единоразова€ утановка приоритета
}


void CBitingRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_STAND;

	pMonster->SetMemoryTimeDef();
	
}

void CBitingRest::Init()
{
	IState::Init();

	// если есть путь - дойти до конца (последстви€ преследовани€ врага)
	if (!pMonster->AI_Path.TravelPath.empty()) {
		m_bFollowPath = true;
	} else m_bFollowPath = false;
}


void CBitingRest::Run()
{
	
	if (m_bFollowPath) {
		if ((pMonster->AI_Path.TravelPath.size() - 1) <= pMonster->AI_Path.TravelStart) m_bFollowPath = false;
	}
	
	if (m_bFollowPath) {
		m_tAction = ACTION_WALK_GRAPH_END;
	} else {
		// проверить нужно ли провести перепланировку
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastPlanTime, m_dwReplanTime);
			Replanning();
		DO_IN_TIME_INTERVAL_END();
	}
	
	// FSM 2-го уровн€
	switch (m_tAction) {
		case ACTION_WALK:		// обход точек графа
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
		case ACTION_STAND:     // сто€ть, ничего не делать
			pMonster->Motion.m_tParams.SetParams(eAnimStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_LIE:		// лежать
			pMonster->Motion.m_tParams.SetParams(eAnimLieIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_TURN:		// повернутьс€ на 90 град.
			pMonster->Motion.m_tParams.SetParams(eAnimStandTurnLeft,0,pMonster->m_ftrStandTurnRSpeed, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
			break;
		case ACTION_WALK_GRAPH_END:
			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
	}
}

void CBitingRest::Replanning()
{
	m_dwLastPlanTime = m_dwCurrentTime;	
	u32		rand_val = ::Random.randI(100);
	u32		cur_val;
	u32		dwMinRand, dwMaxRand;
	
	if (rand_val < (cur_val = pMonster->m_dwProbRestWalkFree)) {	
		m_tAction = ACTION_WALK;
		// ѕостроить путь обхода точек графа
		pMonster->AI_Path.TravelPath.clear();
		pMonster->vfUpdateDetourPoint();	
		pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

		dwMinRand = pMonster->m_timeFreeWalkMin;
		dwMaxRand = pMonster->m_timeFreeWalkMax;

	} else if (rand_val < (cur_val = cur_val + pMonster->m_dwProbRestStandIdle)) {	
		m_tAction = ACTION_STAND;

		dwMinRand = pMonster->m_timeStandIdleMin;
		dwMaxRand = pMonster->m_timeStandIdleMax;
		
	} else if (rand_val < (cur_val = cur_val + pMonster->m_dwProbRestLieIdle)) {	
		m_tAction = ACTION_LIE;
		// проверить лежит уже?
		if (pMonster->m_tAnim != eAnimLieIdle) {
			pMonster->Motion.m_tSeq.Add(eAnimStandLieDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Switch();
		}

		dwMinRand = pMonster->m_timeLieIdleMin;
		dwMaxRand = pMonster->m_timeLieIdleMax;

	} else  {	
		m_tAction = ACTION_TURN;
		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI_DIV_2);

		dwMinRand = 1000;
		dwMaxRand = 1100;

	}
	
	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
	//SetNextThink(dwMinRand);

}


TTime CBitingRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingAttack::CBitingAttack(CAI_Biting *p)  
{
	pMonster = p;
	Reset();
	SetHighPriority();
}


void CBitingAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	m_tEnemy.obj		= 0;
	m_bAttackRat		= false;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

	m_dwFaceEnemyLastTime	= 0;
	m_dwFaceEnemyLastTimeInterval = 1200;
	
	m_dwSuperMeleeStarted	= 0;
}

void CBitingAttack::Init()
{
	IState::Init();

	// ѕолучить врага
	m_tEnemy = pMonster->m_tEnemy;

	// ќпределение класса врага
	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(m_tEnemy.obj);
	if (tpRat) m_bAttackRat = true;
	else m_bAttackRat = false;

	// ”становка дистанции аттаки
	if (m_bAttackRat) {
		m_fDistMin = 0.7f;
		m_fDistMax = 2.8f;
	} else {
		m_fDistMin = 2.4f;
		m_fDistMax = 3.8f;
	}
	
	pMonster->SetMemoryTimeDef();

	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CBitingAttack::Run()
{
	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");

	// ≈сли враг изменилс€, инициализировать состо€ние
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// ¬ыбор состо€ни€
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);
	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);
	
	// задержка дл€ построени€ пути
	u32 delay;

	// если враг не виден - бежать к нему
	if (!m_bAttackRat) {
		if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time != m_dwCurrentTime)) {
			m_tAction = ACTION_RUN;
		}
	}

	// ¬ыполнение состо€ни€
	switch (m_tAction) {	
		case ACTION_RUN:		// бежать на врага
			delay = ((m_bAttackRat)? 0: 300);
			
			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, delay);

			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			// если враг крыса под монстром подпрыгнуть и убить
			if (dist < 0.6f) {
				if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;

				if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
					// прыгнуть
					pMonster->Motion.m_tSeq.Add(eAnimAttackJump,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
					pMonster->Motion.m_tSeq.Switch();
					m_dwSuperMeleeStarted = 0;
				}
			} else m_dwSuperMeleeStarted = 0;
			
			
			// —мотреть на врага 
			float yaw, pitch;
			yaw = pMonster->r_torso_target.yaw;

			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, m_dwFaceEnemyLastTimeInterval);
				
				pMonster->AI_Path.TravelPath.clear();

				Fvector dir;
				dir.sub(m_tEnemy.obj->Position(), pMonster->Position());
				dir.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);

			DO_IN_TIME_INTERVAL_END();

			// set motion params
			if (m_bAttackRat) pMonster->Motion.m_tParams.SetParams(eAnimAttackRat,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			else pMonster->Motion.m_tParams.SetParams(eAnimAttack,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			pMonster->Motion.m_tTurn.Set(eAnimFastTurn, eAnimFastTurn, 0, pMonster->m_ftrAttackFastRSpeed,pMonster->m_ftrRunAttackMinAngle);
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingEat::CBitingEat(CAI_Biting *p)  
{
	pMonster = p;
	Reset();
	SetLowPriority();
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
	IState::Init();

	// ѕолучить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 2.5f);
	
	SavedPos			= pCorpse->Position();		// сохранить позицию трупа
	m_fDistToDrag		= 20.f;
	bDragging			= false;

	m_tAction			= ACTION_RUN;

	// Test
	WRITE_TO_LOG("_ Eat Init _");
}

void CBitingEat::Run()
{
	// ≈сли новый труп, снова инициализировать состо€ние 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) Init();
	
	float cur_dist = SavedPos.distance_to(pMonster->Position()); // рассто€ние, на которое уже оттащен труп

	// ¬ыполнение состо€ни€
	switch (m_tAction) {
		case ACTION_RUN:	// бежать к трупу

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,2000);

			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			if (pCorpse->Position().distance_to(pMonster->Position()) < m_fDistToCorpse) {
				// вз€ть труп
				pMonster->Movement.PHCaptureObject(pCorpse);
				if (pMonster->Movement.PHCapture()->Failed()) {
					bDragging = false;
					m_tAction = ACTION_EAT;
				}else {
					bDragging = true;
					m_tAction = ACTION_DRAG;
				}
				
				// если монстр подбежал к трупу, необходимо отыграть проверку трупа
				pMonster->Motion.m_tSeq.Add(eAnimCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
				pMonster->Motion.m_tSeq.Switch();
			}
			break;
		
		case ACTION_DRAG:
					
			pMonster->m_tEnemy.Set(pCorpse,0);				// forse enemy selection
			pMonster->vfInitSelector(pMonster->m_tSelectorCover, false);
			
			pMonster->m_tSelectorCover.m_fMaxEnemyDistance = cur_dist + pMonster->m_tSelectorCover.m_fSearchRange;
			pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
			pMonster->m_tSelectorCover.m_fMinEnemyDistance = cur_dist + 3.f;

			pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0, 5000);

			// ”становить параметры движени€
			pMonster->Motion.m_tParams.SetParams(eAnimWalkBkwd,pMonster->m_ftrWalkSpeed / 2, pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set		(eAnimWalkBkwd, eAnimWalkBkwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);			
			pMonster->Motion.m_tTurn.SetMoveBkwd(true);

			// если не может тащить
			if (pMonster->Movement.PHCapture() == 0) m_tAction = ACTION_RUN;

			if (cur_dist > m_fDistToDrag) {
				// бросить труп
				pMonster->Movement.PHReleaseObject();

				bDragging = false; 
				m_tAction = ACTION_EAT;
				// лечь
				pMonster->Motion.m_tSeq.Add(eAnimStandLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
				pMonster->Motion.m_tSeq.Switch();
			}
			
			break;
		case ACTION_EAT:
			pMonster->Motion.m_tParams.SetParams(eAnimEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

			if (pMonster->GetSatiety() >= 1.0f) pMonster->flagEatNow = false;
			else pMonster->flagEatNow = true;

			// съесть часть
			DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
				pMonster->ChangeSatiety(0.05f);
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
			DO_IN_TIME_INTERVAL_END();

			break;
	}
}

bool CBitingEat::CheckCompletion()
{	
	return false;
}

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow = false;

	pMonster->Motion.m_tTurn.SetMoveBkwd(false);
	// если тащит труп - бросить
	if (bDragging) {
		pMonster->Movement.PHReleaseObject();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingHide::CBitingHide(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingHide::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);

	// Test
	WRITE_TO_LOG("_ Hide Init _");
}

void CBitingHide::Reset()
{
	inherited::Reset();

	m_tEnemy.obj		= 0;
}

void CBitingHide::Run()
{
	Fvector EnemyPos;
	if (m_tEnemy.obj) EnemyPos = m_tEnemy.obj->Position();
	else EnemyPos = m_tEnemy.position;
	
	pMonster->vfInitSelector(pMonster->m_tSelectorCover, false);
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = EnemyPos.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

	// ”становить параметры движени€
	pMonster->Motion.m_tParams.SetParams	(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingDetour::CBitingDetour(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CBitingDetour::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	WRITE_TO_LOG(" DETOUR init!");
}

void CBitingDetour::Run()
{
	WRITE_TO_LOG("--- DETOUR ---");

	VisionElem tempEnemy;
	if (pMonster->GetEnemy(tempEnemy)) m_tEnemy = tempEnemy;

	pMonster->vfUpdateDetourPoint();
	pMonster->AI_Path.DestNode		= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
	
	pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
	pMonster->m_tSelectorCover.m_fOptEnemyDistance = 15;
	pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.obj->Position().distance_to(pMonster->Position()) + 3.f;
	
	pMonster->vfInitSelector(pMonster->m_tSelectorCover, false);	
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0, 2000);

	// ”становить параметры движени€
	pMonster->Motion.m_tParams.SetParams	(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBitingPanic::Reset()
{
	inherited::Reset();

	m_tEnemy.obj	= 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;

}

void CBitingPanic::Init()
{
	inherited::Init();

	// ѕолучить врага
	m_tEnemy = pMonster->m_tEnemy;

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	// Test
	WRITE_TO_LOG("_ Panic Init _");
}

void CBitingPanic::Run()
{
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);

		pMonster->Motion.m_tSeq.Add(eAnimFastTurn,0,pMonster->m_ftrScaredRSpeed * 2.0f,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);		
		pMonster->Motion.m_tSeq.Switch();
	} 

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;


	pMonster->vfInitSelector(pMonster->m_tSelectorFreeHunting,false);
	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	if (!bFacedOpenArea) {
		pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);
	} else {
		// try to rebuild path
		if (pMonster->AI_Path.TravelPath.size() > 5) {
			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);
		} else {
			pMonster->Motion.m_tParams.SetParams(eAnimStandDamaged,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();
		}
	}
	
	prev_pos = cur_pos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDNE::CBitingExploreDNE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBitingExploreDNE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;

}

void CBitingExploreDNE::Init()
{
	// Test
	WRITE_TO_LOG("_ ExploreDNE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);	// возвращает самый опасный звук
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.position;
	m_tEnemy.time = se.time;

	Fvector dir; 
	dir.sub(m_tEnemy.position,pMonster->Position());
	float yaw,pitch;
	dir.getHP(yaw,pitch);
	
	// проиграть анимацию испуга

	pMonster->Motion.m_tSeq.Add(eAnimFastTurn,0,pMonster->m_ftrScaredRSpeed * 2.0f,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_ANIM_END);
	pMonster->Motion.m_tSeq.Add(eAnimScared,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
	pMonster->Motion.m_tSeq.Switch();

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDNE::Run()
{
	// “икать нафиг
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);

		pMonster->Motion.m_tSeq.Add(eAnimFastTurn,0,pMonster->m_ftrScaredRSpeed * 2.0f,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED,STOP_ANIM_END);		
		pMonster->Motion.m_tSeq.Switch();
	} 

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;


	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	if (!bFacedOpenArea) {
		pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);
	} else {
		pMonster->Motion.m_tParams.SetParams(eAnimStandDamaged,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Clear();
	}

	prev_pos = cur_pos;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreDE::CBitingExploreDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingExploreDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
	m_tAction = ACTION_LOOK_AROUND;
	m_dwTimeToTurn	= 0;	
}

void CBitingExploreDE::Init()
{
	// Test
	WRITE_TO_LOG("_ ExploreDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);	// возвращает самый опасный звук
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.position;
	m_dwSoundTime	  = se.time;

	float	yaw,pitch;
	Fvector dir;
	dir.sub(m_tEnemy.position,pMonster->Position());
	dir.getHP(yaw,pitch);

	pMonster->r_torso_target.yaw = yaw;
	m_dwTimeToTurn = (TTime)(_abs(angle_normalize_signed(yaw - pMonster->r_torso_current.yaw)) / pMonster->m_ftrStandTurnRSpeed * 1000);

	SetInertia(20000);
	pMonster->SetMemoryTime(20000);
}

void CBitingExploreDE::Run()
{
	// определение состо€ни€
	if (m_tAction == ACTION_LOOK_AROUND && (m_dwStateStartedTime + m_dwTimeToTurn < m_dwCurrentTime)) m_tAction = ACTION_HIDE;

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);	// возвращает самый опасный звук
	
	switch(m_tAction) {
	case ACTION_LOOK_AROUND:
		pMonster->Motion.m_tParams.SetParams(eAnimStandTurnLeft,0,pMonster->m_ftrStandTurnRSpeed, pMonster->r_torso_target.yaw, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
		pMonster->Motion.m_tTurn.Clear();
		break;
	case ACTION_HIDE:
		pMonster->m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorCover.m_fSearchRange;
		pMonster->m_tSelectorCover.m_fOptEnemyDistance = pMonster->m_tSelectorCover.m_fMaxEnemyDistance;
		pMonster->m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorCover, 0, true, 0,2000);

		// ”становить параметры движени€
		pMonster->Motion.m_tParams.SetParams	(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set			(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
		break;
	}
}

bool CBitingExploreDE::CheckCompletion()
{	
//	if (!m_tEnemy.obj) return true;
//	
//	// проверить, если ли новый звук, завершить состо€ние
//	SoundElem se;
//	bool bDangerous;
//	pMonster->GetMostDangerousSound(se,bDangerous);	// возвращает самый опасный звук
//	
//	if ((m_dwSoundTime + 2000)< se.time) return true;
//
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreNDE class  
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingExploreNDE::CBitingExploreNDE(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetLowPriority();
}

void CBitingExploreNDE::Reset()
{
	inherited::Reset();
	m_tEnemy.obj = 0;
}

void CBitingExploreNDE::Init()
{
	// Test
	WRITE_TO_LOG("_ ExploreNDE Init _");

	inherited::Init();

	R_ASSERT(pMonster->IsRememberSound());

	SoundElem se;
	bool bDangerous;
	pMonster->GetSound(se,bDangerous);			// возвращает самый опасный звук
	m_tEnemy.obj = dynamic_cast<CEntity *>(se.who);
	m_tEnemy.position = se.position;

	if (m_tEnemy.obj) pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;

	// проиграть анимацию испуга
	SetInertia(6000);
}

void CBitingExploreNDE::Run()
{
	pMonster->vfChoosePointAndBuildPath(0, &m_tEnemy.position, false, 0, 2000);

	// ”становить параметры движени€
	pMonster->Motion.m_tParams.SetParams	(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Set			(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
}




