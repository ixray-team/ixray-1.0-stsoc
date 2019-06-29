
#include "stdafx.h"
#include "ai_chimera.h"
#include "ai_chimera_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CChimeraAttack::CChimeraAttack(CAI_Chimera *p)  
{
	pMonster = p;
	Reset();
	SetHighPriority();
}


void CChimeraAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	m_tEnemy.obj		= 0;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

	m_dwFaceEnemyLastTime	= 0;
	m_dwFaceEnemyLastTimeInterval = 1200;
	
}

void CChimeraAttack::Init()
{
	IState::Init();

	// Получить врага
	m_tEnemy = pMonster->m_tEnemy;

	// Установка дистанции аттаки
	m_fDistMin = 2.4f;
	m_fDistMax = 4.8f;
	
	pMonster->SetMemoryTimeDef();

	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CChimeraAttack::Run()
{
	// Если враг изменился, инициализировать состояние
	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;

	// Выбор состояния
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax))m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// если враг не виден на протяжении 1 сек - бежать к нему
	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time + 1000 < m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}
	
	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:		// бежать на врага
			
			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, 300);

//			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
			// Смотреть на врага 
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

//			pMonster->Motion.m_tParams.SetParams(eAnimAttack,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
//			pMonster->Motion.m_tTurn.Set(eAnimFastTurn, eAnimFastTurn, 0, pMonster->m_ftrAttackFastRSpeed,pMonster->m_ftrRunAttackMinAngle);
			break;
	}
}

bool CChimeraAttack::CheckCompletion()
{	
	return false;
}


