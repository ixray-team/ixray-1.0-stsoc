////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_a_star.cpp
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life A* realization
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_a_star.h"
#include "ai_space.h"
#include "a_star_interfaces.h"
#include "a_star_template.h"

CSE_ALifeAStar::CSE_ALifeAStar			()
{
	m_tpHeap							= 0;
	m_tpIndexes							= 0;
	m_dwAStarStaticCounter				= 0;
	m_tpaNodes.clear					();
};
	
CSE_ALifeAStar::~CSE_ALifeAStar			()
{
	xr_free								(m_tpHeap);
	xr_free								(m_tpIndexes);
	xr_delete							(m_tpAIGraphData);
	xr_delete							(m_tpGraphPath);
}

void CSE_ALifeAStar::Init				()
{
	u32 S1								= (getAI().GraphHeader().dwVertexCount + 2)*sizeof(SNode);
	m_tpHeap							= (SNode *)xr_malloc(S1);
	ZeroMemory							(m_tpHeap,S1);
	u32 S2								= getAI().GraphHeader().dwVertexCount*sizeof(SIndexNode);
	m_tpIndexes							= (SIndexNode *)xr_malloc(S2);
	ZeroMemory							(m_tpIndexes,S2);
	Msg									("* ALife path-finding structures: %d K",(S1 + S2)/(1024));
	m_tpGraphPath						= (void*)xr_new<CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG> >   (getAI().GraphHeader().dwVertexCount + 2);
	m_tpAIGraphData						= xr_new<SAIMapDataG>();
	m_tpAIGraphData->tpGraph			= &(getAI());
}

float CSE_ALifeAStar::ffFindMinimalPath	(u32 dwStartNode, u32 dwGoalNode)
{
	return								(ffFindMinimalPath(dwStartNode, dwGoalNode, m_tpaNodes));
}

float CSE_ALifeAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, DWORD_VECTOR &tpaNodes)
{
	float								fDistance;
	m_tpAIGraphData->dwFinishNode		= dwGoalNode;
	((CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG>*)(m_tpGraphPath))->vfFindOptimalPath	(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,*m_tpAIGraphData,dwStartNode,dwGoalNode,40000.f,fDistance,tpaNodes,false);
	return								(fDistance);
}