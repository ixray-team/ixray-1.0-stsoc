////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_path.cpp
//	Created 	: 28.03.2003
//  Modified 	: 28.03.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../graph_engine.h"
#include "../ai_monsters_misc.h"

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 2000
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f

//void CAI_Stalker::vfSearchForBetterPosition(CAbstractVertexEvaluator & /**tNodeEvaluator/**/, CSquad & /**Squad/**/, CEntity* & /**Leader/**/)
//{
//	Device.Statistic.AI_Range.Begin();	// определение времени вып. функции
//	
//	if ((!m_previous_query_time) || (CDetailPathManager::m_path.empty()) || (int(CDetailPathManager::m_current_travel_point) > int(CDetailPathManager::m_path.size()) - 4) || (speed() < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_previous_query_time > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
//		
//		m_previous_query_time = tNodeEvaluator.m_dwCurTime;
//		
//		float fOldCost = MAX_NODE_ESTIMATION_COST;
//		
//		if (ai().level_graph().valid_vertex_id(m_level_dest_vertex_id)) {
//			tNodeEvaluator.m_tpCurrentNode	= ai().level_graph().vertex(m_level_dest_vertex_id);
//			tNodeEvaluator.m_fDistance		= Position().distance_to(ai().level_graph().vertex_position(m_level_dest_vertex_id));
//			fOldCost						= tNodeEvaluator.ffEvaluateNode();
////			Msg								("Old  : [%d][%f]",level_vertex_id(),fOldCost);
//		}
//
//		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);
//		
//		Device.Statistic.AI_Range.End();
//		
//		tNodeEvaluator.vfShallowGraphSearch(ai().q_mark_bit);
//		
//		Device.Statistic.AI_Range.Begin();
//
////		Msg									("Best : [%d][%f]",tNodeEvaluator.m_dwBestNode,tNodeEvaluator.m_fBestCost);
////		Msg									("Params : %f - [%f][%f][%f][%f][%f][%f]",m_tEnemy.m_object->Position().distance_to(Position()),tNodeEvaluator.m_fMaxEnemyDistance,tNodeEvaluator.m_fOptEnemyDistance,tNodeEvaluator.m_fMinEnemyDistance,tNodeEvaluator.m_fMaxEnemyDistanceWeight,tNodeEvaluator.m_fOptEnemyDistanceWeight,tNodeEvaluator.m_fMinEnemyDistanceWeight);
////		Msg									("Evaluator : [%f][%f][%f]",tNodeEvaluator.m_fMaxEnemyDistance,tNodeEvaluator.m_fOptEnemyDistance,tNodeEvaluator.m_fMinEnemyDistance);
//		if ((m_level_dest_vertex_id != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
//			m_level_dest_vertex_id		= tNodeEvaluator.m_dwBestNode;
//			if (!m_level_dest_vertex_id) {
//				Msg("! Invalid vertex Evaluator vertex");
//			}
//			m_level_path.clear		();
//			m_tPathState			= ePathStateBuildNodePath;
//			m_bIfSearchFailed		= false;
//		} 
//		else
//			m_bIfSearchFailed		= true;
//	}
//
//	Device.Statistic.AI_Range.End();
//}

//void CAI_Stalker::vfBuildPathToDestinationPoint(CAbstractVertexEvaluator * /**tpNodeEvaluator/**/)
//{
//	Device.Statistic.AI_Path.Begin();
//	
//	if (m_level_dest_vertex_id == level_vertex_id()) {
//		m_level_path.clear		();
//		CDetailPathManager::m_path.clear();
//		CDetailPathManager::m_current_travel_point		= 0;
//		m_fCurSpeed				= 0;
//		m_tPathState			= ePathStateSearchNode;
//		Device.Statistic.AI_Path.End();
//		return;
//	}
//	
//	if (tpNodeEvaluator)
//		ai().m_tpAStar->ffFindOptimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
//	else
//		ai().m_tpAStar->ffFindMinimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path);
//	
//	if (m_level_path.empty()) {
//#ifdef DEBUG
//		Msg("! !!!! node_start %d, node_finish %d",level_vertex_id(),m_level_dest_vertex_id);
//#endif
////		if (tpNodeEvaluator) {
//			ai().m_tpAStar->ffFindMinimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path);
//			if (m_level_path.empty())
//				m_tPathState = ePathStateSearchNode;
//			else
//				m_tPathState = ePathStateBuildTravelLine;
////		}
////		else
////			m_tPathState = ePathStateSearchNode;
//	}
//	else
//		m_tPathState = ePathStateBuildTravelLine;
//	
//	Device.Statistic.AI_Path.End();
//}

//void CAI_Stalker::vfBuildTravelLine(Fvector * /**tpDestinationPosition/**/)
//{
//	Device.Statistic.TEST1.Begin();
//	
//	if (m_tPathType == ePathTypeCriteria) {
//		AI_Path.BuildTravelLine	(Position());
//		m_tPathState			= ePathStateSearchNode;
//		CDetailPathManager::m_current_travel_point		= 0;
//	}
//	else {
//		if (tpDestinationPosition && ai().level_graph().inside(ai().level_graph().vertex(m_level_dest_vertex_id),*tpDestinationPosition))
//			tpDestinationPosition->y = ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(m_level_dest_vertex_id),tpDestinationPosition->x,tpDestinationPosition->z);
//		m_tpaPoints.clear			();
//		m_tpaDeviations.clear		();
//		m_tpaTravelPath.clear		();
//		m_tpaPointNodes.clear		();
//
//		u32							N = (int)m_level_path.size();
//		if (!N) {
//			Msg("! vertex list is empty!");
//			m_level_path.clear		();
//			CDetailPathManager::m_path.clear();
//			m_tPathState			= ePathStateSearchNode;
//			Device.Statistic.TEST1.End();
//			return;
//		}
//		Fvector						tStartPosition = Position();
//		u32							dwCurNode = level_vertex_id();
//		m_tpaPoints.push_back		(Position());
//		m_tpaPointNodes.push_back	(dwCurNode);
//
//		for (u32 i=1; i<=N; ++i)
//			if (i<N) {
//				if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,m_level_path[i])) {
//					if (dwCurNode != m_level_path[i - 1])
//						m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[--i]));
//					else
//						m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[i]));
//					m_tpaPointNodes.push_back(dwCurNode);
//				}
//			}
//			else
//				if (tpDestinationPosition)
//					if (ai().level_graph().check_position_in_direction(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
//						//VERIFY(false);
//						if (dwCurNode != m_level_dest_vertex_id)
//							m_tpaPointNodes.push_back(m_level_dest_vertex_id);
//						m_tpaPoints.push_back(*tpDestinationPosition);
//					}
//					else {
//						//if (dwCurNode != m_level_dest_vertex_id)
//						dwCurNode = m_level_dest_vertex_id;
//						if (ai().level_graph().inside(ai().level_graph().vertex(dwCurNode),*tpDestinationPosition)) {
//							tpDestinationPosition->y = ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(m_level_dest_vertex_id),tpDestinationPosition->x,tpDestinationPosition->z);
//							m_tpaPointNodes.push_back(m_level_dest_vertex_id);
//							m_tpaPoints.push_back(*tpDestinationPosition);
//						}
//					}
//		
//		if (!tpDestinationPosition && (tStartPosition.distance_to(ai().level_graph().vertex_position(m_level_path[N - 1])) > ai().level_graph().header().cell_size())) {
//			m_tpaPoints.push_back(ai().level_graph().vertex_position(m_level_path[N - 1]));
//			m_tpaPointNodes.push_back(m_level_path[N - 1]);
//		}
//		
//		m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());
//		
//		if (m_tPathType == ePathTypeStraight)
//			CDetailPathManager::m_path.clear();
//		else
//			m_tpaTempPath.clear();
//		
//		m_level_path.clear		();
//		
//		AI::CTravelNode	T;
//		T.floating		= false;
//		T.P.set			(0,0,0);
//		for (i=1; i<N; ++i) {
//			m_tpaLine.clear();
//			m_tpaLine.push_back(m_tpaPoints[i-1]);
//			m_tpaLine.push_back(m_tpaPoints[i]);
//			//ai().vfCreateFastRealisticPath(m_tpaLine,m_tpaPointNodes[i-1],m_tpaDeviations,m_tpaTravelPath,m_tpaNodes,false,false,0,0);
//			ai().bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
//			u32 n = (u32)m_tpaTravelPath.size();
//			for (u32 j= 0; j<n; ++j) {
//				T.P = m_tpaTravelPath[j];
//				if (m_tPathType == ePathTypeStraight)
//					CDetailPathManager::m_path.push_back(T);
//				else
//					m_tpaTempPath.push_back(T.P);
//				m_level_path.push_back(m_tpaNodes[j]);
//			}
//		}
//		if ((N > 1) && (m_tPathType == ePathTypeStraight)) {
//			m_level_path[m_level_path.size() - 1] = m_level_dest_vertex_id;
//			if (tpDestinationPosition && CDetailPathManager::m_path.size() && CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L) {
//				if (ai().level_graph().inside(ai().level_graph().vertex(m_level_dest_vertex_id),*tpDestinationPosition) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(m_level_dest_vertex_id,T.P,*tpDestinationPosition))) {
//					T.P = *tpDestinationPosition;
//					CDetailPathManager::m_path.push_back(T);
//				}
//			}
//		}
//		else
//			if (tpDestinationPosition && CDetailPathManager::m_path.size() && CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L)
//				if (ai().level_graph().inside(ai().level_graph().vertex(m_level_dest_vertex_id),*tpDestinationPosition) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(m_level_dest_vertex_id,T.P,*tpDestinationPosition)))
//					m_tpaTempPath.push_back(*tpDestinationPosition);
//		
//		if (m_tPathType == ePathTypeStraight) {
//			CDetailPathManager::m_current_travel_point = 0;
//			m_tPathState		= ePathStateSearchNode;
//		}
//		else
//			m_tPathState		= ePathStateDodgeTravelLine;
//	}
//	
//	Device.Statistic.TEST1.End();
//}

//void CAI_Stalker::vfDodgeTravelLine()
//{
//	//Device.Statistic.TEST0.Begin();
//
//	int							N = (int)m_tpaTempPath.size();
//	CDetailPathManager::m_path.resize	(N);
//	Fvector						tLeft, tRight, tStartPosition;
//	
//	if (N) {
//		tStartPosition						= m_tpaTempPath[N - 1];
//		tStartPosition.sub					(Position());
//		float								yaw, pitch;
//		tStartPosition.getHP				(yaw,pitch);
//		
//		yaw									+= PI_DIV_2;
//		tStartPosition.setHP				(yaw,pitch);
//		tStartPosition.normalize_safe		();
//		tLeft								= tStartPosition;
//		
//		yaw									+= PI;
//		tStartPosition.setHP				(yaw,pitch);
//		tStartPosition.normalize_safe		();
//		tRight								= tStartPosition;
//		
//		CDetailPathManager::m_path[0].floating		= FALSE;
//		CDetailPathManager::m_path[0].P				= m_tpaTempPath[0];
//		CDetailPathManager::m_path[N - 1].floating	= FALSE;
//		CDetailPathManager::m_path[N - 1].P			= m_tpaTempPath[N - 1];
//		for (int i=1; i < N - 1; ++i)
//			if (i & 1) {
//				Fvector						tPoint;
//				if ((i % 4) == 1) {
//					float					fDistance = DODGE_AMPLITUDE*_min(m_tpaTempPath[i].distance_to(m_tpaTempPath[i - 1]),MAX_DODGE_DISTANCE);
//					tPoint					= tLeft;
//					tPoint.mul				(fDistance);
//					tPoint.add				(m_tpaTempPath[i]);
//				}
//				else {
//					float				fDistance = DODGE_AMPLITUDE*_min(m_tpaTempPath[i].distance_to(m_tpaTempPath[i - 1]),MAX_DODGE_DISTANCE);
//					tPoint				= tRight;
//					tPoint.mul			(fDistance);
//					tPoint.add			(m_tpaTempPath[i]);
//				}
//				
//				u32							dwNode1, dwNode2;
//				dwNode1						= ai().level_graph().check_position_in_direction(m_level_path[i - 1],m_tpaTempPath[i - 1],tPoint);
//				dwNode2						= ai().level_graph().check_position_in_direction(m_level_path[i + 1],m_tpaTempPath[i + 1],tPoint);
//
//				CDetailPathManager::m_path[i].floating	= FALSE;
//				
//				if (ai().level_graph().valid_vertex_id(dwNode1) && ai().level_graph().valid_vertex_id(dwNode2)) {
//					CDetailPathManager::m_path[i].P		= tPoint;
//					CDetailPathManager::m_path[i].P.y	= ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(dwNode1),CDetailPathManager::m_path[i].P.x,CDetailPathManager::m_path[i].P.z);
//				}
//				else
//					CDetailPathManager::m_path[i].P		= m_tpaTempPath[i];
//			}
//			else {
//				CDetailPathManager::m_path[i].floating	= FALSE;
//				CDetailPathManager::m_path[i].P			= m_tpaTempPath[i];
//			}
//	}
//	
//	CDetailPathManager::m_current_travel_point			= 0;
//	m_tPathState				= ePathStateSearchNode;
//
//	//Device.Statistic.TEST0.End();
//}

//void CAI_Stalker::vfChoosePointAndBuildPath(CAbstractVertexEvaluator * /**tpNodeEvaluator/**/, Fvector * /**tpDestinationPosition/**/, bool /**bSearchForNode/**/)
//{
//	INIT_SQUAD_AND_LEADER;
//
//	if (m_tPrevPathType != m_tPathType) {
//		m_tPrevPathType		= m_tPathType;
//		m_tPathState		= ePathStateSearchNode;
//		m_level_path.clear	();
//	}
//	if (tpNodeEvaluator)
//		vfInitSelector			(*tpNodeEvaluator,Squad,Leader);
//		
//	EPathType tPathType = m_tPathType;
//	switch (m_tPathType) {
//		case ePathTypeStraight : {
//			m_tPathType = ePathTypeStraight;
//			break;
//		}
//		case ePathTypeDodge : {
//			m_tPathType = ePathTypeDodge;
//			break;
//		}
//		case ePathTypeCriteria : {
//			m_tPathType = ePathTypeCriteria;
//			break;
//		}
//		case ePathTypeStraightDodge : {
//			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
//				m_tPathType = ePathTypeStraight;
//			else
//				m_tPathType = ePathTypeDodge;
//			break;
//		}
//		case ePathTypeDodgeCriteria : {
//			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
//				m_tPathType = ePathTypeDodge;
//			else
//				m_tPathType = ePathTypeCriteria;
//			break;
//		}
//	}
//	switch (m_tPathState) {
//		case ePathStateSearchNode : {
//			if (tpNodeEvaluator && bSearchForNode)
//				vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
//			else
//				if (!bSearchForNode || !tpDestinationPosition || !CDetailPathManager::m_path.size() || (CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
//					m_tPathState = ePathStateBuildNodePath;
//			break;
//		}
//		case ePathStateBuildNodePath : {
//			if ((m_level_dest_vertex_id != level_vertex_id()) && (m_level_path.empty() || (m_level_path[m_level_path.size() - 1] != m_level_dest_vertex_id) || CDetailPathManager::m_path.empty() || ((CDetailPathManager::m_path.size() - 1) <= CDetailPathManager::m_current_travel_point)))
//				vfBuildPathToDestinationPoint(tpNodeEvaluator);
//			else
//				if ((m_level_dest_vertex_id == level_vertex_id()) && tpDestinationPosition) {
//					m_level_path.clear();
//					m_level_path.push_back(level_vertex_id());
//					m_tPathState = ePathStateBuildTravelLine;
//				}
//				else
//					if (bSearchForNode && tpNodeEvaluator)
//						m_tPathState = ePathStateSearchNode;
//
//			break;
//		}
//		case ePathStateBuildTravelLine : {
//			vfBuildTravelLine(tpDestinationPosition);
//			break;
//		}
//		case ePathStateDodgeTravelLine : {
//			vfDodgeTravelLine();
//			break;
//		}
//	}
//	m_tPathType = tPathType;
//}

//void CAI_Stalker::vfMarkVisibleNodes(CEntity * /**tpEntity/**/)
//{
//	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
//	if (!tpCustomMonster)
//		return;
//
//	Fvector tDirection;
//	float fRange = tpCustomMonster->ffGetRange();
//	
//	for (float fIncrement = 0; fIncrement < PI_MUL_2; fIncrement += PI/10.f) {
//		tDirection.setHP(fIncrement,0.f);
//		if (tpEntity->level_vertex_id() >= ai().level_graph().header().vertex_count())
//			continue;
//		ai().ffMarkNodesInDirection(tpCustomMonster->level_vertex_id(),tpCustomMonster->Position(),tDirection,fRange,ai().m_dwaNodeStackM,&ai().m_baNodeMarks);
//		Device.Statistic.TEST2.End	();
//		Device.Statistic.AI_Think.End	();
//#pragma todo("Dima to Dima : Instead of slice implement time-delay computations")
//		Engine.Sheduler.Slice		();
//		Device.Statistic.AI_Think.Begin	();
//		Device.Statistic.TEST2.Begin();
//	}
//}

//void CAI_Stalker::vfFindAllSuspiciousNodes(u32 /**StartNode/**/, Fvector /**tPointPosition/**/, const Fvector& /**BasePos/**/, float /**Range/**/, CGroup &/**Group/**/)
//{
//	Device.Statistic.TEST3.Begin	();
//
//	Group.m_tpaSuspiciousNodes.clear();
//	if (StartNode > ai().level_graph().header().vertex_count())
//		return;
//
//	BOOL bStop = FALSE;
//	
//	CLevelGraph &AI = ai();
//	CLevelGraph::CPosition QueryPos;
//	AI.level_graph().vertex_position(QueryPos,BasePos);
//
//	AI.m_dwaNodeStackM.clear();
//	AI.m_dwaNodeStackS.clear();
//	AI.m_dwaNodeStackS.push_back(StartNode);
//	AI.m_baNodeMarks[StartNode] = true;
//	
//	float range_sqr		= Range*Range;
//	float fEyeFov;
//	CEntityAlive *tpEnemy = dynamic_cast<CEntityAlive*>(m_tSavedEnemy);
//	if (tpEnemy)
//		fEyeFov = tpEnemy->ffGetFov();
//	else
//		fEyeFov = ffGetFov();
//	fEyeFov *=PI/180.f;
//
//	INIT_SQUAD_AND_LEADER;
//	
//	Device.Statistic.TEST3.End	();
//	Device.Statistic.TEST2.Begin	();
//	for (int i=0; i<(int)Group.Members.size(); ++i)
//		vfMarkVisibleNodes(Group.Members[i]);
//	Device.Statistic.TEST2.End	();
//	Device.Statistic.TEST3.Begin	();
//
//	for (u32 it=0; it<AI.m_dwaNodeStackS.size(); ++it) {
//		u32 ID = AI.m_dwaNodeStackS[it];
//
//		CLevelGraph::CVertex*	N = AI.vertex(ID);
//		u32 L_count	= u32(N->links);
//		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(CLevelGraph::CVertex));
//		NodeLink* L_end	= L_it+L_count;
//		
//		for( ; L_it!=L_end; ++L_it) {
//			if (bStop)			
//				break;
//			// test vertex
//			u32 Test = AI.UnpackLink(*L_it);
//			if (AI.m_baNodeMarks[Test])
//				continue;
//
//			CLevelGraph::CVertex*	T = AI.vertex(Test);
//
//			float distance_sqr = AI.u_SqrDistance2Node(BasePos,T);
//			if (distance_sqr>range_sqr)	
//				continue;
//
//			// register
//			AI.m_baNodeMarks[Test]		= true;
//			AI.m_dwaNodeStackS.push_back(Test);
//
//			// estimate
//			SSearchPlace tSearchPlace;
//
//			Fvector tDirection, tNodePosition = AI.level_graph().vertex_position(T);
//			tDirection.sub(tPointPosition,tNodePosition);
//			tDirection.normalize_safe();
//			SRotation tRotation;
//			mk_rotation(tDirection,tRotation);
//			float m_cost = ai().level_graph().cover_in_direction(tRotation.yaw,T);
//			
//			if (m_cost < .6f) {
//				bool bOk = false;
//				float fMax = 0.f;
//				for (int i=0, iIndex = -1; i<(int)Group.m_tpaSuspiciousNodes.size(); ++i) {
//					Fvector tP0 = AI.level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID);
//					float fDistance = tP0.distance_to(tNodePosition);
//					if (fDistance < 10.f) {
//						if (fDistance < 3.f) {
//							if (m_cost < Group.m_tpaSuspiciousNodes[i].m_cost) {
//								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
//								Group.m_tpaSuspiciousNodes[i].m_cost = m_cost;
//							}
//							bOk = true;
//							break;
//						}
//						tDirection.sub(tP0,tNodePosition);
//						tDirection.normalize_safe();
//						mk_rotation(tDirection,tRotation);
//						if (ai().level_graph().cover_in_direction(tRotation.yaw,T) > .3f) {
//							if (m_cost < Group.m_tpaSuspiciousNodes[i].m_cost) {
//								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
//								Group.m_tpaSuspiciousNodes[i].m_cost = m_cost;
//							}
//							bOk = true;
//							break;
//						}
//					}
//					if (m_cost > fMax) {
//						fMax = Group.m_tpaSuspiciousNodes[i].m_cost;
//						iIndex = i;
//					}
//				}
//				if (!bOk) {
//					if (Group.m_tpaSuspiciousNodes.size() < MAX_SUSPICIOUS_NODE_COUNT) {
//						tSearchPlace.dwNodeID	= Test;
//						tSearchPlace.m_cost		= m_cost;
//						tSearchPlace.dwSearched	= 0;
//						tSearchPlace.dwGroup	= 0;
//						Group.m_tpaSuspiciousNodes.push_back(tSearchPlace);
//					}
//					else
//						if ((fMax > m_cost) && (iIndex >= 0)) {
//							Group.m_tpaSuspiciousNodes[iIndex].dwNodeID = Test;
//							Group.m_tpaSuspiciousNodes[iIndex].m_cost = m_cost;
//						}
//				}
//			}
//		}
//		if (bStop)
//			break;
//#pragma todo("Dima to Dima : Instead of slice implement time-delay computations")
//		Device.Statistic.TEST3.End	();
//		Device.Statistic.AI_Think.End	();
//		Engine.Sheduler.Slice			();
//		Device.Statistic.AI_Think.Begin	();
//		Device.Statistic.TEST3.Begin	();
//	}
//
//	{
//		xr_vector<u32>::iterator it		= AI.m_dwaNodeStackS.begin();
//		xr_vector<u32>::iterator end	= AI.m_dwaNodeStackS.end();
//		for ( ; it!=end; ++it)	
//			AI.m_baNodeMarks[*it] = false;
//		
//		it	= ai().m_dwaNodeStackM.begin();
//		end	= ai().m_dwaNodeStackM.end();
//		for ( ; it!=end; ++it)	
//			AI.m_baNodeMarks[*it] = false;
//	}
//	Device.Statistic.TEST3.End();
//}

#define GROUP_RADIUS	15.f

//void CAI_Stalker::vfClasterizeSuspiciousNodes(CGroup &/**Group/**/)
//{
// 	u32 N = Group.m_tpaSuspiciousNodes.size();
//	for (int i=0, iGroupCounter = 1; i<(int)N; ++i, ++iGroupCounter) {
//		if (!Group.m_tpaSuspiciousNodes[i].dwGroup) 
//			Group.m_tpaSuspiciousNodes[i].dwGroup = iGroupCounter;
//		for (int j=0; j<(int)N; ++j)
//			if (!Group.m_tpaSuspiciousNodes[j].dwGroup && (ai().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[j].dwNodeID,Group.m_tpaSuspiciousNodes[i].dwNodeID) < GROUP_RADIUS))
//				Group.m_tpaSuspiciousNodes[j].dwGroup = iGroupCounter;
//	}
//	for ( i=0; i<(int)N; ++i)
//		--(Group.m_tpaSuspiciousNodes[i].dwGroup);
//	Group.m_tpaSuspiciousGroups.resize(--iGroupCounter);
//	for ( i=0; i<iGroupCounter; ++i)
//		Group.m_tpaSuspiciousGroups[i] = 0;
//}

//void CAI_Stalker::vfChooseSuspiciousNode(CAbstractVertexEvaluator &/**tSelector/**/)
//{
//	CGroup &Group = *getGroup();
//	INIT_SQUAD_AND_LEADER;
//	if (m_iCurrentSuspiciousNodeIndex >= (int)Group.m_tpaSuspiciousNodes.size())
//		m_iCurrentSuspiciousNodeIndex = -1;
//	if ((speed() < EPS_L) || (!m_bActionStarted)) {
//		if ((-1 != m_iCurrentSuspiciousNodeIndex) && (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID == level_vertex_id()))
//			Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 2;
//		if ((m_iCurrentSuspiciousNodeIndex == -1) || (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched == 2))
//			m_iCurrentSuspiciousNodeIndex = ifGetSuspiciousAvailableNode(m_iCurrentSuspiciousNodeIndex,Group);
//		if (-1 != m_iCurrentSuspiciousNodeIndex) {
//			Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 1;
//			Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwGroup] = 1;
//			vfInitSelector(tSelector,Squad,Leader);
//			tSelector.m_tpEnemyNode = ai().level_graph().vertex(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID);
//			tSelector.m_tEnemyPosition = ai().level_graph().vertex_position(tSelector.m_tpEnemyNode);
//			m_level_dest_vertex_id = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//			if (!m_level_dest_vertex_id) {
//				Msg("! Invalid suspicious point vertex");
//			}
//			vfBuildPathToDestinationPoint(0);
//			m_bActionStarted = true;
//		}
//		else {
//			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
//			m_tSelectorRetreat.m_tpEnemyNode = ai().level_graph().vertex(m_dwMyNodeID);
//			m_tSelectorRetreat.m_tMyPosition = Position();
//			m_tSelectorRetreat.m_tpMyNode = level_vertex();
//			vfChoosePointAndBuildPath(&m_tSelectorRetreat);
//			m_iCurrentSuspiciousNodeIndex = -1;
//			m_bActionStarted = true;
//		}
//	}
//	else {
//		for (int i=0, iCount = 0; i<(int)Group.m_tpaSuspiciousNodes.size(); ++i)
//			if (2 != Group.m_tpaSuspiciousNodes[i].dwSearched) {
//				if ((Group.m_tpaSuspiciousNodes[i].dwNodeID == level_vertex_id()) || bfCheckForNodeVisibility(Group.m_tpaSuspiciousNodes[i].dwNodeID, i == m_iCurrentSuspiciousNodeIndex))
//					Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
//				++iCount;
//			}
//		if ((-1 != m_iCurrentSuspiciousNodeIndex) && (Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched == 2)) {
//			CDetailPathManager::m_path.clear();
//			if (!iCount && (m_current_update - m_dwLostEnemyTime > TIME_TO_SEARCH)) {
//				m_tSavedEnemy = 0;
//				//GO_TO_PREV_STATE;
//			}
//			else {
//				m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
//				m_tSelectorRetreat.m_tpEnemyNode = ai().level_graph().vertex(m_dwMyNodeID);
//				m_tSelectorRetreat.m_tMyPosition = Position();
//				m_tSelectorRetreat.m_tpMyNode = level_vertex();
//				vfChoosePointAndBuildPath(&m_tSelectorRetreat);
//				m_iCurrentSuspiciousNodeIndex = -1;
//				m_bActionStarted = true;
//			}
//		}
//		else
//			if (-1 != m_iCurrentSuspiciousNodeIndex) {
//				if (m_level_dest_vertex_id != Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID) {
//					m_level_dest_vertex_id = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//					if (!m_level_dest_vertex_id) {
//						Msg("! Invalid suspicious point vertex");
//					}
//					vfBuildPathToDestinationPoint(0);
//				}
//			}
//	}
//}

//int CAI_Stalker::ifGetSuspiciousAvailableNode(int /**iLastIndex/**/, CGroup &/**Group/**/)
//{
//	int Index = -1;
//	float fMin = 1000, m_cost;
//	u32 dwNodeID = level_vertex_id();
//	if (iLastIndex >= 0) {
//		dwNodeID = Group.m_tpaSuspiciousNodes[iLastIndex].dwNodeID;
//		int iLastGroup = Group.m_tpaSuspiciousNodes[iLastIndex].dwGroup;
//		for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); ++i) {
//			if (((int)Group.m_tpaSuspiciousNodes[i].dwGroup != iLastGroup) || Group.m_tpaSuspiciousNodes[i].dwSearched)
//				continue;
//			if ((m_cost = ai().ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
//				fMin = m_cost;
//				Index = i;
//			}
//		}
//	}
//	
//	if (Index >= 0)
//		return(Index);
//
//	for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); ++i) {
//		if (Group.m_tpaSuspiciousNodes[i].dwSearched)
//			continue;
//		if (Group.m_tpaSuspiciousNodes[i].m_cost < fMin) {
//			fMin = Group.m_tpaSuspiciousNodes[i].m_cost;
//			Index = i;
//		}
//	}
//
//	if (Index == -1) {
//		for (int i=0; i<(int)Group.m_tpaSuspiciousNodes.size(); ++i) {
//			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
//				continue;
//			if (Group.m_tpaSuspiciousNodes[i].m_cost < fMin) {
//				fMin = Group.m_tpaSuspiciousNodes[i].m_cost;
//				Index = i;
//			}
//		}
//	}
//	return(Index);
//	return(0);
//}