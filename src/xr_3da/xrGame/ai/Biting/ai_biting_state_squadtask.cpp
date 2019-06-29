#include "stdafx.h"

//CBitingSquadTask::CBitingSquadTask(CAI_Biting *p)
//{
//	pMonster = p;
//
//}
//
//void CBitingSquadTask::Init()
//{
//	inherited::Init();
//
//	//// Get task
//	//CMonsterSquad *pSquad = Level().SquadMan->GetSquad((u8)pMonster->g_Squad());
//	//GTask &task = pSquad->GetTask(pMonster);
//
//	//switch (task.state.command) {
//	//case SC_EXPLORE:		break;
//	//case SC_ATTACK:			break;
//	//case SC_THREATEN:		break;
//	//case SC_COVER:			
//	//	// strict follow		
//	//		
//	//	LOG_EX("COVER...");
//
//	//	break;
//
//	//case SC_FOLLOW:			
//	//	
//	//	LOG_EX("FOLLOWING INIT...");
//
//	//	
//	//	break;
//	//case SC_FEEL_DANGER:	
//	//	
//	//	
//	//	break;
//	//}
//
//	//flag_once_1 = false;
//}
//
//void CBitingSquadTask::Run()
//{
//	//// Get task
//	//CMonsterSquad *pSquad = Level().SquadMan->GetSquad((u8)pMonster->g_Squad());
//	//GTask &task = pSquad->GetTask(pMonster);
//
//	//switch (task.state.command) {
//	//case SC_ATTACK:			break;
//	//case SC_THREATEN:		break;
//	//case SC_COVER:			
//	//	// if (dist to big) walk to
//	//	// else stand and look at target
//	//	
//	//	{
//
//	//	R_ASSERT(task.target.entity);
//
//	//	Fvector pos = task.target.entity->Position();
//	//	float dist = pMonster->Position().distance_to(pos);
//
//	//	if (dist > 3.f) {
//	//		
//	//		DO_ONCE_BEGIN(flag_once_1);
//	//			saved_pos = random_position(pos,5.f);	
//	//		DO_ONCE_END();
//	//		
//	//		if (!pMonster->CDetailPathManager::path().empty()) {
//	//			Fvector p = pMonster->CDetailPathManager::path()[pMonster->CDetailPathManager::path().size()-1].position;
//	//			if ((pMonster->Position().distance_to(p) <  1.f) || 
//	//				(pMonster->CDetailPathManager::completed(pMonster->Position()))) saved_pos = random_position(pos,5.f);
//	//		} else saved_pos = random_position(pos,5.f);
//
//	//		pMonster->MoveToTarget(saved_pos);	
//	//		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
//	//		pMonster->CMovementManager::set_path_type (CMovementManager::ePathTypeLevelPath);
//
//	//	} else {
//	//		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
//	//		pMonster->LookPosition(pos);
//	//	}
//
//	//	
//	//	}
//
//	//	break;
//
//	//case SC_FEEL_DANGER:
//	//	
//	//	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
//	//	pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
//
//	//	break;
//	//case SC_FOLLOW:			
//	//	// Exec path selector
//	//	pMonster->MoveToTarget(task.target.pos);
//
//	//	LOG_EX("Squad Task	::	Evaluate selector... ");
//
//	//	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
//
//	//	break;
//
//	//case SC_EXPLORE:  {
//	//	// 1. Идти к позиции, осмотреться || проверка трупа
//	//	float cur_dist=0.0f;
//	//	
//	//	// Get cur dist
//	//	if (task.target.entity) {
//
//	//		CPhysicsShellHolder *pO = smart_cast<CPhysicsShellHolder *>(task.target.entity);
//	//		R_ASSERT(pO);
//	//		Fvector nearest_bone_pos = pMonster->m_PhysicMovementControl->PHCaptureGetNearestElemPos(pO);
//	//		cur_dist = nearest_bone_pos.distance_to(pMonster->Position());
//	//			
//	//	} else { 
//	//		cur_dist = pMonster->Position().distance_to(task.target.pos);
//	//	}
//	//	
//	//	if (cur_dist > 1.f) {
//
//	//		// Set movement 
//	//		if (task.target.entity) {
//	//			CEntity *pE = smart_cast<CEntity *>(task.target.entity);
//	//			R_ASSERT(pE);
//	//			//pMonster->MoveToTarget(pE);
//	//		} else { 
//	//			//pMonster->MoveToTarget(task.target.pos, task.target.node);
//	//		}
//
//	//		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
//
//	//	} else {
//	//		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
//	//	}
//	//	
//	//				  }
//	//	break;
//	//}
//}
//
//

