#include "stdafx.h"
#include "ai_space.h"

u32 CAI_Space::q_Node(u32 PrevNode, const Fvector& BasePos, bool bShortSearch)
{
	if (0==vfs)	return	0;
	
	Device.Statistic.AI_Node.Begin	();
	
	// Check if we still in old node
	NodePosition	QueryPos;
	PackPosition	(QueryPos,BasePos);

	if (PrevNode < m_header.count) {
		if (u_InsideNode(*m_nodes_ptr[PrevNode],QueryPos)) {
		//if (bfInsideNode(BasePos,m_nodes_ptr[PrevNode])) {
			Device.Statistic.AI_Node.End();
			return PrevNode;
		}
		
		// Perform neibourhood search
		CAI_NodeEvaluatorTemplate<aiSearchRange | aiInsideNode> tSearch;
		tSearch.m_fSearchRange = m_header.size*3;
		tSearch.m_dwStartNode = PrevNode;
		tSearch.m_tStartPosition = BasePos;
		tSearch.vfShallowGraphSearch(getAI().q_mark_bit);
		if (getAI().u_InsideNode(*getAI().Node(tSearch.m_dwBestNode),QueryPos))	{
			// small distance from node
			Device.Statistic.AI_Node.End();
			return tSearch.m_dwBestNode;
		}
		
		tSearch.m_fSearchRange = m_header.size*30;
		tSearch.vfShallowGraphSearch(getAI().q_mark_bit);
	//	q_Range_Bit(PrevNode,BasePos,m_header.size*30,&QueryPos,BestNode,BestCost);
		if (tSearch.m_fBestCost <= 30*30*m_header.size*m_header.size)	{
			// small distance from node
			Device.Statistic.AI_Node.End();
			return tSearch.m_dwBestNode;
		}
	}
	if (bShortSearch && PrevNode && (PrevNode < m_header.count)) {
		Device.Statistic.AI_Node.End		();
		return(PrevNode);
	}
	// degrade to linear search
	int id = q_LoadSearch(BasePos);
	if (id>0) return u32(id);
	
	// everything failed :(
	// maintain old node
	Device.Statistic.AI_Node.End		();
	return PrevNode;
}