////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"
#include "..\\..\\a_star.h"
#include "..\\ai_monsters_misc.h"

using namespace AI_Chimera;
using namespace AI;

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 4000		// 2 сек
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f

/////////////////////////////////////////////////////////////////////////////////////
// Функция InitSelector

void CAI_Chimera::vfInitSelector(IBaseAI_NodeEvaluator &S, CSquad &Squad)
{
	S.m_dwCurTime		= m_dwCurrentUpdate;
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();


	VisionElem ve;
	GetEnemy(ve);
	//R_ASSERT(ve.obj);
	if (!ve.obj) return;

	S.m_tEnemy			= ve.obj;
	S.m_tEnemyPosition	= ve.position;
	S.m_dwEnemyNode		= ve.node_id;
	S.m_tpEnemyNode		= ve.node;

	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= AI_NodeID;		// текущий узел
	S.m_tStartPosition	= Position();
}

/////////////////////////////////////////////////////////////////////////////////////
// Функция SearchForBetterPosition
void CAI_Chimera::vfSearchForBetterPosition(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader)
{
	Device.Statistic.AI_Range.Begin();	// определение времени вып. функции

	if ((!m_dwLastRangeSearch) || (AI_Path.TravelPath.empty()) || (int(AI_Path.TravelStart) > int(AI_Path.TravelPath.size()) - 4) || (AI_Path.fSpeed < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
	//   если запускается в первый раз || нет пути             ||  

		m_dwLastRangeSearch = tNodeEvaluator.m_dwCurTime;

		float fOldCost = MAX_NODE_ESTIMATION_COST;

		if (AI_Path.DestNode != u32(-1)) {
			tNodeEvaluator.m_tpCurrentNode	= getAI().Node(AI_Path.DestNode);
			tNodeEvaluator.m_fDistance		= Position().distance_to(getAI().tfGetNodeCenter(AI_Path.DestNode));
			fOldCost						= tNodeEvaluator.ffEvaluateNode();
		}

		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);

		Device.Statistic.AI_Range.End();

		tNodeEvaluator.vfShallowGraphSearch(getAI().q_mark_bit);

		Device.Statistic.AI_Range.Begin();

		if ((AI_Path.DestNode != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
			AI_Path.DestNode		= tNodeEvaluator.m_dwBestNode;
			if (!AI_Path.DestNode) {
				Msg("! Invalid Node Evaluator node");
			}
			AI_Path.Nodes.clear		();
			m_tPathState			= ePathStateBuildNodePath;
			m_bIfSearchFailed		= false;
		} 
		else
			m_bIfSearchFailed		= true;
	}

	Device.Statistic.AI_Range.End();
}

/////////////////////////////////////////////////////////////////////////////////////
// Функция BuildPathToDestinationPoint
void CAI_Chimera::vfBuildPathToDestinationPoint(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	Device.Statistic.AI_Path.Begin();

	if (AI_Path.DestNode == AI_NodeID) {
		AI_Path.Nodes.clear		();
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart		= 0;
		m_fCurSpeed				= 0;
		m_tPathState			= ePathStateSearchNode;
		Device.Statistic.AI_Path.End();
		return;
	}

	if (tpNodeEvaluator)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);

	if (AI_Path.Nodes.empty()) {
		Msg("! !!!! node_start %d, node_finish %d",AI_NodeID,AI_Path.DestNode);
		
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
		
		if (AI_Path.Nodes.empty())
			m_tPathState = ePathStateSearchNode;
		else
			m_tPathState = ePathStateBuildTravelLine;
	}
	else
		m_tPathState = ePathStateBuildTravelLine;

	Device.Statistic.AI_Path.End();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Построить TravelLine
void CAI_Chimera::vfBuildTravelLine(Fvector *tpDestinationPosition)
{
	Device.Statistic.TEST1.Begin();

	m_tpaPoints.clear			();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	u32							N = (u32)AI_Path.Nodes.size();
	if (!N) {
		Msg("! Node list is empty!");
		AI_Path.Nodes.clear();
		AI_Path.TravelPath.clear();
		m_tPathState = ePathStateSearchNode;
		Device.Statistic.TEST1.End();
		return;
	}
	Fvector						tStartPosition = Position();

	u32							dwCurNode = AI_NodeID;
	m_tpaPoints.push_back		(Position());
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; i++)
		if (i<N) {
			if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
				if (dwCurNode != AI_Path.Nodes[i - 1])
					m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (tpDestinationPosition)
				if (getAI().dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
					//VERIFY(false);
					if (dwCurNode != AI_Path.DestNode)
						m_tpaPointNodes.push_back(AI_Path.DestNode);
					tpDestinationPosition->y = getAI().ffGetY(*getAI().Node(dwCurNode),tpDestinationPosition->x,tpDestinationPosition->z);
					m_tpaPoints.push_back(*tpDestinationPosition);
				}
				else {
						tpDestinationPosition->y = getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tpDestinationPosition->x,tpDestinationPosition->z);
						m_tpaPointNodes.push_back(AI_Path.DestNode);
						m_tpaPoints.push_back(*tpDestinationPosition);
				}

	if (!tpDestinationPosition && (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size)) {
		m_tpaPoints.push_back(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1]));
		m_tpaPointNodes.push_back(AI_Path.Nodes[N - 1]);
	}

	N = (u32)m_tpaPoints.size();

	AI_Path.TravelPath.clear();
	AI_Path.Nodes.clear		();

	AI::CTravelNode	T;
	T.floating		= false;
	T.P.set			(0,0,0);
	for (i=1; i<N; i++) {
		m_tpaLine.clear();
		m_tpaLine.push_back(m_tpaPoints[i-1]);
		m_tpaLine.push_back(m_tpaPoints[i]);
		getAI().bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
		u32 n = (u32)m_tpaTravelPath.size();
		for (u32 j= 0; j<n; j++) {
			T.P = m_tpaTravelPath[j];
			AI_Path.TravelPath.push_back(T);
			AI_Path.Nodes.push_back(m_tpaNodes[j]);
		}
	}
	
	if (AI_Path.Nodes.size())
		AI_Path.Nodes[AI_Path.Nodes.size() - 1] = AI_Path.DestNode;
	else
		AI_Path.Nodes.push_back(AI_Path.DestNode);
	
	if ((N > 1) && tpDestinationPosition && AI_Path.TravelPath.size() && AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L) {
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),*tpDestinationPosition) && getAI().dwfCheckPositionInDirection(AI_Path.DestNode,T.P,*tpDestinationPosition) != -1) {
			T.P = *tpDestinationPosition;
			AI_Path.TravelPath.push_back(T);
		}
	}

	AI_Path.TravelStart = 0;
	m_tPathState		= ePathStateSearchNode;

	Device.Statistic.TEST1.End();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Выбор точки, построение пути, построение TravelLine
void CAI_Chimera::vfChoosePointAndBuildPath(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode, bool bSelectorPath, u32 TimeToRebuild)
{
	if (m_tPathState == ePathStateBuilt) {
		if (m_dwPathBuiltLastTime + TimeToRebuild > m_dwCurrentUpdate) return;
		m_tPathState = ePathStateSearchNode;
	}


	INIT_SQUAD_AND_LEADER;

	if (tpNodeEvaluator) {
		vfInitSelector			(*tpNodeEvaluator,Squad);
		if (!tpNodeEvaluator->m_tEnemy) return;
	}


	Fvector tempV;

	switch (m_tPathState) {
		case ePathStateSearchNode : 
			if (tpNodeEvaluator && bSearchForNode)  // необходимо искать ноду?
				vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
			else
				if (!bSearchForNode || !tpDestinationPosition || !AI_Path.TravelPath.size() || (AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
					m_tPathState = ePathStateBuildNodePath;	// 
			break;
									
		case ePathStateBuildNodePath : 
			if ((AI_Path.DestNode != AI_NodeID) && (AI_Path.Nodes.empty() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode) || AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)))
				vfBuildPathToDestinationPoint(bSelectorPath ? tpNodeEvaluator : 0);
			else
				if ((AI_Path.DestNode == AI_NodeID) && tpDestinationPosition && (!AI_Path.TravelPath.size() || (tpDestinationPosition->distance_to_xz(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) > EPS_L))) {
					AI_Path.Nodes.clear();
					AI_Path.Nodes.push_back(AI_NodeID);
					m_tPathState = ePathStateBuildTravelLine;
				}
				else
					if (bSearchForNode && tpNodeEvaluator)
						m_tPathState = ePathStateSearchNode;
					else
						if (AI_Path.TravelPath.size() && tpDestinationPosition && (AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
							m_tPathState = ePathStateBuildTravelLine;
			break;
									
		case ePathStateBuildTravelLine : 
			
			tempV = ((tpDestinationPosition) ?  *tpDestinationPosition : getAI().tfGetNodeCenter(AI_Path.Nodes.back()));
			//vfBuildTravelLine(tempV,Position());
			vfBuildTravelLine(&tempV);

			m_tPathState = ePathStateBuilt;
			m_dwPathBuiltLastTime = m_dwCurrentUpdate;
			break;
										
	}	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Выбор следующей точки графа
void CAI_Chimera::vfChooseNextGraphPoint()
{
	_GRAPH_ID						tGraphID		= m_tNextGP;
	u16								wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge		*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);

	int								iPointCount		= (int)m_tpaTerrain.size();
	int								iBranches		= 0;	// количество различных вариантов
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP		= m_tNextGP;
					m_tNextGP		= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
			R_ASSERT2(false,"There are no proper graph neighbours!");
		}
	}
	else {
		int							iChosenBranch = ::Random.randI(0,iBranches);
		iBranches					= 0;
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						m_tCurGP	= m_tNextGP;
						m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
						return;
					}
					iBranches++;
				}
		}
	}
}

// Выбор точки графа, в соответствии с выбором лидера
void CAI_Chimera::vfUpdateDetourPoint()
{
	if (!g_Alive())
		return;

	INIT_SQUAD_AND_LEADER;

	if (this != Leader)	{
		CAI_Chimera *tpLeader			= dynamic_cast<CAI_Chimera*>(Leader);
		if (tpLeader) {
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
			m_tNextGP					= tpLeader->m_tNextGP;
		}
	}
	else
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
			m_dwTimeToChange			= Level().timeServer() + 2000;
		}
		
}

