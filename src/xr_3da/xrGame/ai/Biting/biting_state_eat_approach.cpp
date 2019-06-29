
#include "stdafx.h"
#include "ai_biting.h"
#include "biting_state_eat_approach.h"
#include "../ai_monster_mem.h"


CStateBitingEatApproach::CStateBitingEatApproach(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatApproach::~CStateBitingEatApproach()
{
}


void CStateBitingEatApproach::initialize()
{
	inherited::initialize();
	
	cur_dist = flt_max;
}

void CStateBitingEatApproach::execute()
{
	VisionElem ve;
	if (!m_object->GetCorpse(ve) || (!ve.obj->m_pPhysicsShell)) {return;}
	
//	const CEntity *pCorpse = ve.obj;
	
//	// Определить позицию ближайшей боны у трупа
//	Fvector nearest_bone_pos;
//	if (dynamic_cast<CGameObject *>(pCorpse)->m_pPhysicsShell == NULL) {
//		nearest_bone_pos	= pCorpse->Position(); 
//	} else nearest_bone_pos = m_object->m_PhysicMovementControl->PHCaptureGetNearestElemPos(const_cast<CEntity*>(pCorpse));
//
//	// вычислить дистанцию до ближайшей боны
//	cur_dist = nearest_bone_pos.distance_to(m_object->Position());
//
//	// определить ноду для выбранной позиции
//	Fvector		approach_pos;
//	u32			approach_vertex_id;
//	bool		bNeedRebuild = false; 
//	
//	approach_vertex_id = ai().level_graph().check_position_in_direction(pCorpse->level_vertex_id(), pCorpse->Position(), nearest_bone_pos);
//	if (approach_vertex_id == -1) {
//		approach_vertex_id	= pCorpse->level_vertex_id();
//		approach_pos		= ai().level_graph().vertex_position(approach_vertex_id);
//	} else {
//		approach_pos = nearest_bone_pos;
//	}
//	
//	R_ASSERT(ai().level_graph().inside(approach_vertex_id,approach_pos));
//
//	// перестроить путь
//	if (IS_NEED_REBUILD()) bNeedRebuild = true;
//	if (m_object->CMovementManager::level_dest_vertex_id() != approach_vertex_id) bNeedRebuild = true;
//	
//	// установить параметры
//	if (bNeedRebuild) m_object->MoveToTarget(approach_pos,approach_vertex_id);
//	m_object->MotionMan.m_tAction		= ACT_RUN;
//	m_object->MotionMan.accel_activate	(eAT_Calm);
//	m_object->CSoundPlayer::play(MonsterSpace::eMonsterSoundIdle, 0,0,m_object->_sd->m_dwIdleSndDelay);																																						BOOL WANT_TO_EAT = FALSE; R_ASSERT(WANT_TO_EAT, "BITTING CREATURE IS NOT HUNGRY!!!!");
}

void CStateBitingEatApproach::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatApproach::completed() const
{
	if (cur_dist < m_object->_sd->m_fDistToCorpse) return true;
	return false;
}

