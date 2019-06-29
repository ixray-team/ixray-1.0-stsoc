////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star_interfaces.h
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Interfaces for A* algortihm
////////////////////////////////////////////////////////////////////////////

#pragma once

//this file is included for performance purposes - to inline functions that uses CAI_Space class
#include "ai_space.h"

struct SAIMapData {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
};

struct SAIMapDataL {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	float		fLight;
	float		fCover;
	float		fDistance;
};

struct SAIMapDataE {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	u32			dwEnemyNode;
	float		fLight;
	float		fCover;
	float		fDistance;
	float		fEnemyDistance;
	float		fEnemyView;
};

struct SAIMapDataF {
	CAI_Space	*tpAI_Space;
	u32			dwFinishNode;
	Fvector		tEnemyPosition;
	float		fLight;
	float		fCover;
	float		fDistance;
	float		fEnemyDistance;
	float		fEnemyView;
};

struct SAIMapDataG {
	CSE_ALifeGraph	*tpGraph;
	u32			dwFinishNode;
};


class CAIMapTemplateNode {
public:
	float		x1;
	float		y1;
	float		z1;
	float		x2;
	float		y2;
	float		z2;
	float		x3;
	float		y3;
	float		z3;
	u32			m_dwLastBestNode;
	typedef		NodeLink* iterator;
};

class CAIGraphTemplateNode {
public:
	u32			m_dwLastBestNode;
	typedef		CSE_ALifeGraph::SGraphEdge* iterator;
	u32			get_value				(iterator &tIterator)
	{
		return(tIterator->dwVertexNumber);
	}
};

class CAIMapShortestPathNode : public CAIMapTemplateNode {
public:
	SAIMapData	tData;
				
				CAIMapShortestPathNode	(SAIMapData &tAIMapData)
	{
		tData					= tAIMapData;
		m_dwLastBestNode		= u32(-1);
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
		x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	}

	IC	void	begin					(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
	{
		tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
	}

	IC	u32		get_value				(CAIMapTemplateNode::iterator &tIterator)
	{
		return(tData.tpAI_Space->UnpackLink(*tIterator));
	}

	IC	bool	bfCheckIfAccessible		(u32 dwNode)
	{
		return(tData.tpAI_Space->q_mark_bit[dwNode]);
	}

	IC	float	ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		if (m_dwLastBestNode != dwStartNode) {
			m_dwLastBestNode = dwStartNode;
			NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
			
			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
	}

	IC	float	ffAnticipate			(u32 dwStartNode)
	{
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
		
		x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
	}

	IC	float	ffAnticipate			()
	{
		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
	}
};

class CAIMapLCDPathNode : public CAIMapTemplateNode {
public:
	SAIMapDataL	tData;
	float		m_fSum;
				
				CAIMapLCDPathNode		(SAIMapDataL &tAIMapData)
	{
		tData					= tAIMapData;
		m_dwLastBestNode		= u32(-1);
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
		x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
		float fLight			= (float)(tNode1.light)/255.f;
		m_fSum					= fCover + fLight;
	}

	IC	void	begin					(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
	{
		tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
	}

	IC	u32		get_value				(CAIMapTemplateNode::iterator &tIterator)
	{
		return(tData.tpAI_Space->UnpackLink(*tIterator));
	}

	IC	bool	bfCheckIfAccessible		(u32 dwNode)
	{
		return(tData.tpAI_Space->q_mark_bit[dwNode]);
	}

	IC	float	ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		if (m_dwLastBestNode != dwStartNode) {
			m_dwLastBestNode = dwStartNode;
			NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
	}

	IC	float	ffAnticipate			(u32 dwStartNode)
	{
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);

		x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}

	IC	float	ffAnticipate			()
	{
		return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}
};

class CAIMapEnemyPathNode : public CAIMapTemplateNode {
public:
	float		x4;
	float		y4;
	float		z4;
	SAIMapDataE	tData;
	float		m_fSum;
				
				CAIMapEnemyPathNode		(SAIMapDataE &tAIMapData)
	{
		tData					= tAIMapData;
		m_dwLastBestNode		= u32(-1);
		{
			NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwEnemyNode);
			x4						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y4						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z4						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		}
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
		x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
		float fLight			= (float)(tNode1.light)/255.f;
		m_fSum					= fCover + fLight + tData.fEnemyView*(tData.fEnemyDistance);
	}

	IC	void	begin					(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
	{
		tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
	}

	IC	u32		get_value				(CAIMapTemplateNode::iterator &tIterator)
	{
		return(tData.tpAI_Space->UnpackLink(*tIterator));
	}

	IC	bool	bfCheckIfAccessible		(u32 dwNode)
	{
		return(tData.tpAI_Space->q_mark_bit[dwNode]);
	}

	IC	float	ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		if (m_dwLastBestNode != dwStartNode) {
			m_dwLastBestNode = dwStartNode;
			NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
			
			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(tData.fEnemyView*_abs(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(tData.fEnemyView*_abs(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
	}

	IC	float	ffAnticipate			(u32 dwStartNode)
	{
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
		
		x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		return(m_fSum + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}		   
			   
	IC	float	ffAnticipate			()
	{		   
		return(m_fSum + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}
};

class CAIMapEnemyPositionPathNode : public CAIMapTemplateNode {
public:
	float		x4;
	float		y4;
	float		z4;
	SAIMapDataF	tData;
	float		m_fSum;

				CAIMapEnemyPositionPathNode(SAIMapDataF &tAIMapData)
	{
		tData					= tAIMapData;
		m_dwLastBestNode		= u32(-1);
		{
			x4						= tData.tEnemyPosition.x;
			y4						= tData.tEnemyPosition.y;
			z4						= tData.tEnemyPosition.z;
		}
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
		x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
		float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
		float fLight			= (float)(tNode1.light)/255.f;
		m_fSum					= fCover + fLight + tData.fEnemyView*(tData.fEnemyDistance);
	}

	IC	void	begin					(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
	{
		tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
	}

	IC	u32		get_value				(CAIMapTemplateNode::iterator &tIterator)
	{
		return(tData.tpAI_Space->UnpackLink(*tIterator));
	}

	IC	bool	bfCheckIfAccessible		(u32 dwNode)
	{
		return(tData.tpAI_Space->q_mark_bit[dwNode]);
	}

	IC	float	ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		if (m_dwLastBestNode != dwStartNode) {
			m_dwLastBestNode = dwStartNode;
			NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
			
			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(tData.fEnemyView*_abs(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

			float fLight = (float)(tNode1.light)/255.f;
			
			return(tData.fEnemyView*_abs(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
		}
	}

	IC	float	ffAnticipate			(u32 dwStartNode)
	{
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
		
		x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		return(m_fSum + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}		   
			   
	IC	float	ffAnticipate			()
	{		   
		return(m_fSum + tData.fDistance*(float)_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
	}
};

class CAIGraphShortestPathNode : public CAIGraphTemplateNode {
public:
	SAIMapDataG	tData;
	CSE_ALifeGraph::SGraphVertex			*m_tpaGraph;

				CAIGraphShortestPathNode(SAIMapDataG &tAIMapData)
	{
		tData = tAIMapData;
		m_dwLastBestNode = u32(-1);
		m_tpaGraph = tData.tpGraph->m_tpaGraph;
	}

	IC	void	begin					(u32 dwNode, CAIGraphTemplateNode::iterator &tIterator, CAIGraphTemplateNode::iterator &tEnd)
	{
		tIterator = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[dwNode].dwEdgeOffset);
		tEnd = tIterator + m_tpaGraph[dwNode].tNeighbourCount;
	}

	IC	bool	bfCheckIfAccessible		(u32 dwNode)
	{
		return(true);
	}

	IC	float	ffEvaluate				(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		m_dwLastBestNode = dwFinishNode;
		return(tIterator->fPathDistance);
	}

	IC	float	ffAnticipate			(u32 dwStartNode)
	{
		return(m_tpaGraph[dwStartNode].tGlobalPoint.distance_to(m_tpaGraph[tData.dwFinishNode].tGlobalPoint));
	}

	IC	float	ffAnticipate			()
	{
		return(m_tpaGraph[m_dwLastBestNode].tGlobalPoint.distance_to(m_tpaGraph[tData.dwFinishNode].tGlobalPoint));
	}
};