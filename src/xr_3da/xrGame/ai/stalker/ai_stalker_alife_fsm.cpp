////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife_fsm.cpp
//	Created 	: 15.09.2003
//  Modified 	: 15.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife FSM
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"

void CAI_Stalker::vfContinueWithALifeGoals(CAbstractVertexEvaluator *tpNodeEvaluator)
{
//	set_sound_mask		(u32(eStalkerSoundMaskAnySound));
//
//	// going via graph nodes
//	//WRITE_TO_LOG			("Accomplishing task");
//	if (m_bStateChanged || CMovementManager::path().empty() || (!CMovementManager::actual())) {
//		m_tActionState = !::Random.randI(1) ? eActionStateWatch : eActionStateDontWatch;
//		m_dwActionStartTime = Level().timeServer() + ::Random.randI(30000,50000);
//	}
//
//	if (!bfAssignDestinationNode()) {
//		switch (m_tTaskState) {
//			case ALife::eTaskStateGoingToSearchItem : {
//				m_tTaskState = ALife::eTaskStateAccomplishTask;
//				break;
//											   }
//			case ALife::eTaskStateGoingToCustomer : {
//				m_tTaskState = ALife::eTaskStateTradeWithCustomer;
//				break;
//											 }
//			default : NODEFAULT;
//		}
//		return;
//	}
//
//	if (ai().cross_table().vertex(ai().game_graph().vertex(game_dest_vertex_id())->level_vertex_id()).game_vertex_id() != game_dest_vertex_id()) {
//		string4096	S;
//		sprintf		(S,"Graph doesn't correspond to the cross table (graph vertex %d != CrossTable[Graph[%d].vertex(%d)].Vertex(%d)",
//			game_dest_vertex_id(),
//			game_dest_vertex_id(),
//			ai().game_graph().vertex(game_dest_vertex_id())->level_vertex_id(),
//			ai().cross_table().vertex(ai().game_graph().vertex(game_dest_vertex_id())->level_vertex_id()).game_vertex_id()
//		);
//		R_ASSERT2	(false,S);
//	}
//	
//	if (!ai().level_graph().valid_vertex_id(level_dest_vertex_id())) {
//		Msg("! Invalid graph point vertex (graph index %d)",game_dest_vertex_id());
//		for (ALife::_GRAPH_ID i=0; i<(int)ai().game_graph().header().vertex_count(); ++i)
//			Msg("%3d : %6d",i,ai().game_graph().vertex(i)->level_vertex_id());
//	}
//
//	float					yaw,pitch;
//	GetDirectionAngles		(yaw,pitch);
//	yaw						= angle_normalize_signed(-yaw);
////	if (angle_difference(m_body.current.yaw,yaw) > PI_DIV_6)
////		vfSetParameters(tpNodeEvaluator,0,false,eObjectActionIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypePathDirection);
////	else
////		vfSetParameters(tpNodeEvaluator,0,false,eObjectActionIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypeSearch);
//	vfSetParameters(tpNodeEvaluator,0,0,MonsterSpace::eObjectActionIdle,ePathTypeLevelPath,eDetailPathTypeSmooth,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypeSearch);
//
//	switch (m_tActionState) {
//		case eActionStateWatch : {
//			if (m_dwActionStartTime < Level().timeServer())
//				m_tActionState = eActionStateDontWatch;
//			break;
//		}
//		case eActionStateDontWatch : {
//			if (playing_sounds().empty()) {
//				play				(eStalkerSoundHumming);
//				m_tActionState		= eActionStateWatch;
//				m_dwActionStartTime = Level().timeServer() + ::Random.randI(10000,20000);
//			}
//			break;
//		}
//	}
}

void CAI_Stalker::vfSearchObject()
{
//	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
//		m_tpCurrentSound->stop();
//		m_tpCurrentSound = 0;
//	}
//
//	m_bPlayHumming = false;
//
//	if (level_vertex_id() == m_level_dest_vertex_id) {
//		if (!::Random.randI(10)) {
//			vfFinishTask();
//			return;
//		}
//		Fvector	l_tDirection;
//		l_tDirection.sub(ai().game_graph().vertex(m_tGraphID).level_point(),Position());
//		vfNormalizeSafe(l_tDirection);
//		
//		float y,p;
//		l_tDirection.getHP(y,p);
//		l_tDirection.setHP(y + ::Random.randF(-PI_DIV_2,PI_DIV_2),p);
//		ai().vfFindGraphPointNodeInDirection(level_vertex_id(),Position(),l_tDirection,m_level_dest_vertex_id,m_tGraphID);
//		if (level_vertex_id() == m_level_dest_vertex_id) {
//			if ((ai().m_tpaGraph[m_tGraphID].tNodeID != level_vertex_id()) && !::Random.randI(10))
//				m_level_dest_vertex_id = ai().game_graph().vertex(m_tGraphID).level_vertex_id();
//			else
//				return;
//		}
//	}
//
//	vfSetParameters	(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateFree,eLookTypeSearch);
}