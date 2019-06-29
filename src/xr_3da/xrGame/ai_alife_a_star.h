////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_a_star.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life A* realization
////////////////////////////////////////////////////////////////////////////

#pragma once

struct SNode;
struct SIndexNode;
struct SAIMapDataG;

class CSE_ALifeAStar {
public:
	SNode									*m_tpHeap;
	SIndexNode								*m_tpIndexes;
	u32										m_dwAStarStaticCounter;
	xr_vector<u32>							m_tpaNodes;
	SAIMapDataG								*m_tpAIGraphData;
	void									*m_tpGraphPath;

											CSE_ALifeAStar		();
	virtual									~CSE_ALifeAStar		();
	virtual void							Init				();
	float									ffFindMinimalPath	(u32 dwStartNode, u32 dwGoalNode);
	float									ffFindMinimalPath	(u32 dwStartNode, u32 dwGoalNode, xr_vector<u32> &tpaNodes);
};