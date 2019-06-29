////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "a_star.h"

CAStar::CAStar(CAI_Space *tpAI_Space, u32 dwCount1, u32 dwCount2, u32 dwMaxNodes)
{
	m_tpHeap					= 0;
	m_tpIndexes					= 0;
	m_dwAStarStaticCounter		= 0;
	m_tpaNodes.clear			();
	u32 S1						= (_min(dwMaxNodes,_max(dwCount1,dwCount2)) + 2)*sizeof(SNode);
	m_tpHeap					= (SNode *)xr_malloc(S1);
	Memory.mem_fill				(m_tpHeap,0,S1);
	u32 S2						= _max(dwCount1,dwCount2)*sizeof(SIndexNode);
	m_tpIndexes					= (SIndexNode *)xr_malloc(S2);
	Memory.mem_fill				(m_tpIndexes,0,S2);
	Msg							("* AI path-finding structures: %d K",(S1 + S2)/(1024));
	m_tpMapPath					= xr_new<CAStarSearch<CAIMapShortestPathNode,SAIMapData> >		(_min(dwMaxNodes,dwCount1) + 2);
	m_tpLCDPath					= xr_new<CAStarSearch<CAIMapLCDPathNode,SAIMapDataL> >			(_min(dwMaxNodes,dwCount1) + 2);
	m_tpEnemyPath				= xr_new<CAStarSearch<CAIMapEnemyPathNode,SAIMapDataE> >		(_min(dwMaxNodes,dwCount1) + 2);
	m_tpEnemyPositionPath		= xr_new<CAStarSearch<CAIMapEnemyPositionPathNode,SAIMapDataF> >(_min(dwMaxNodes,dwCount1) + 2);
	m_tpGraphPath				= xr_new<CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG> >   (dwCount2 + 2);
	m_tAIMapData.tpAI_Space		= 
	m_tAIMapDataL.tpAI_Space	= 
	m_tAIMapDataE.tpAI_Space	= 
	m_tAIMapDataF.tpAI_Space	= tpAI_Space;
	m_tAIGraphData.tpGraph		= tpAI_Space;
}

CAStar::~CAStar()
{
	xr_free(m_tpHeap);
	xr_free(m_tpIndexes);
	xr_delete(m_tpMapPath);
	xr_delete(m_tpLCDPath);
	xr_delete(m_tpEnemyPath);
	xr_delete(m_tpEnemyPositionPath);
	xr_delete(m_tpGraphPath);
}

//////////////////////////////////////////////////////////////////////////
// Optimal paths
//////////////////////////////////////////////////////////////////////////

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode)
{
	return(ffFindMinimalPath(dwStartNode,dwGoalNode,m_tpaNodes));
}

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, DWORD_VECTOR &tpaNodes)
{
	float									fDistance;
	m_tAIGraphData.dwFinishNode				= dwGoalNode;
	m_tpGraphPath->vfFindOptimalPath		(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIGraphData,dwStartNode,dwGoalNode,40000.f,fDistance,tpaNodes,false);
	return									(fDistance);
}

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath& Result, bool bUseMarks)
{
	float									fDistance;
	m_tAIMapData.dwFinishNode				= dwGoalNode;
	m_tpMapPath->vfFindOptimalPath			(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIMapData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return									(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath& Result, float fLightWeight, float fCoverWeight, float fDistanceWeight, bool bUseMarks)
{
	float									fDistance;
	m_tAIMapDataL.dwFinishNode				= dwGoalNode;
	m_tAIMapDataL.fLight					= fLightWeight;
	m_tAIMapDataL.fCover					= fCoverWeight;
	m_tAIMapDataL.fDistance					= fDistanceWeight;
	m_tpLCDPath->vfFindOptimalPath			(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIMapDataL,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return									(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath& Result, u32 dwEnemyNode, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	float									fDistance;
	m_tAIMapDataE.dwFinishNode				= dwGoalNode;
	m_tAIMapDataE.fLight					= fLightWeight;
	m_tAIMapDataE.fCover					= fCoverWeight;
	m_tAIMapDataE.fDistance					= fDistanceWeight;
	m_tAIMapDataE.dwEnemyNode				= dwEnemyNode;
	m_tAIMapDataE.fEnemyDistance			= fOptimalEnemyDistance;
	m_tAIMapDataE.fEnemyView				= fEnemyViewWeight;
	m_tpEnemyPath->vfFindOptimalPath		(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIMapDataE,dwStartNode,dwGoalNode,1000000.f,fDistance,Result.Nodes,bUseMarks);
	return									(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	float									fDistance;
	m_tAIMapDataF.dwFinishNode				= dwGoalNode;
	m_tAIMapDataF.fLight					= fLightWeight;
	m_tAIMapDataF.fCover					= fCoverWeight;
	m_tAIMapDataF.fDistance					= fDistanceWeight;
	m_tAIMapDataF.tEnemyPosition			= tEnemyPosition;
	m_tAIMapDataF.fEnemyDistance			= fOptimalEnemyDistance;
	m_tAIMapDataF.fEnemyView				= fEnemyViewWeight;
	m_tpEnemyPositionPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIMapDataF,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return									(fDistance);
}
