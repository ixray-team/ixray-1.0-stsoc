#include "stdafx.h"
#include "ai_space.h"

#define COEFFICIENT	0.001

//void CAI_Space::q_Range(u32 StartNode, const Fvector& BasePos, float Range, IBaseAI_NodeEvaluator& Estimator, float &fOldCost)
//{
////	if (0==vfs)	return;
////
////	Device.Statistic.AI_Range.Begin	();
////
////	// Initialize
////	BOOL bStop = FALSE;
////	NodePosition QueryPos;
////	PackPosition(QueryPos,BasePos);
////	q_stack.clear();
////	q_stack.push_back(StartNode);
////	q_mark [StartNode]	+= 1;
////	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
////	Estimator.BestNode	= StartNode;
////	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
////	Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
////	float range_sqr		= Range*Range;
////
////	// Cycle
////	for (u32 it=0; it<q_stack.size(); it++) {
////		u32 ID = q_stack[it];
////		NodeCompressed*	N = m_nodes_ptr	[ID];
////		u32 L_count	= u32(N->links);
////		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
////		NodeLink* L_end	= L_it+L_count;
////		for( ; L_it!=L_end; L_it++) {
////			if (bStop)			
////				break;
////			// test node
////			u32 Test = UnpackLink(*L_it);
////			if (q_mark[Test])
////				continue;
////
////			NodeCompressed*	T = m_nodes_ptr[Test];
////
////			float distance_sqr = u_SqrDistance2Node(BasePos,T);
////			if (distance_sqr>range_sqr)	
////				continue;
////
////			// register
////			q_mark[Test]		+= 1;
////			q_stack.push_back	(Test);
////
////			// estimate
////			float cost = Estimator.Estimate(T,distance_sqr,bStop);
////			
////			if (cost<Estimator.BestCost) {
////				Estimator.BestCost	= cost;
////				Estimator.BestNode	= Test;
////			}
////		}
////		if (bStop)
////			break;
////	}
////
////	// Clear q_marks
////	{
////		vector<u32>::iterator it	= q_stack.begin();
////		vector<u32>::iterator end	= q_stack.end();
////		for ( ; it!=end; it++)	
////			q_mark[*it] -= 1;
////	}
////
////	Device.Statistic.AI_Range.End();
//}
//
//void CAI_Space::q_Range_Bit(u32 StartNode, const Fvector& BasePos, float Range, IBaseAI_NodeEvaluator& Estimator, float &fOldCost)
//{
////	if (0==vfs)	return;
////
////	Device.Statistic.AI_Range.Begin	();
////
////	// Initialize
////	BOOL bStop = FALSE;
////	NodePosition QueryPos;
////	PackPosition(QueryPos,BasePos);
////	q_stack.clear();
////	q_stack.push_back(StartNode);
////	q_mark_bit [StartNode]	= true;
////	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
////	Estimator.BestNode	= StartNode;
////	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
////	Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
////	float range_sqr		= Range*Range;
////
////	// Cycle
////	for (u32 it=0; it<q_stack.size(); it++) {
////		u32 ID = q_stack[it];
////		NodeCompressed*	N = m_nodes_ptr	[ID];
////		u32 L_count	= u32(N->links);
////		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
////		NodeLink* L_end	= L_it+L_count;
////		for( ; L_it!=L_end; L_it++) {
////			if (bStop)			
////				break;
////			// test node
////			u32 Test = UnpackLink(*L_it);
////			if (q_mark_bit[Test])
////				continue;
////
////			NodeCompressed*	T = m_nodes_ptr[Test];
////
////			float distance_sqr = u_SqrDistance2Node(BasePos,T);
////			if (distance_sqr>range_sqr)	
////				continue;
////
////			// register
////			q_mark_bit[Test]	= true;
////			q_stack.push_back	(Test);
////
////			// estimate
////			float cost = Estimator.Estimate(T,distance_sqr,bStop);
////			
////			if (cost<Estimator.BestCost) {
////				Estimator.BestCost	= cost;
////				Estimator.BestNode	= Test;
////			}
////		}
////		if (bStop)
////			break;
////	}
////
////	// Clear q_marks
////	{
////		vector<u32>::iterator it	= q_stack.begin();
////		vector<u32>::iterator end	= q_stack.end();
////		for ( ; it!=end; it++)	
////			q_mark_bit[*it] = false;
////	}
////
////	Device.Statistic.AI_Range.End();
//}
//
//void CAI_Space::q_Range(u32 StartNode, const Fvector& BasePos, float Range, IBaseAI_NodeEvaluator& Estimator, float &fOldCost, u32 dwTimeDifference)
//{
////	if (0==vfs)	return;
////
////	Device.Statistic.AI_Range.Begin	();
////
////	// Initialize
////	BOOL bStop = FALSE;
////	NodePosition QueryPos;
////	PackPosition(QueryPos,BasePos);
////	q_stack.clear();
////	q_stack.push_back(StartNode);
////	q_mark [StartNode]	+= 1;
////	NodeCompressed*	Base = m_nodes_ptr	[StartNode];
////	Estimator.BestNode	= StartNode;
////	Estimator.BestCost = MAX_NODE_ESTIMATION_COST;
////	Estimator.BestCost = Estimator.Estimate(Base,u_SqrDistance2Node(BasePos,Base),bStop);
////	Estimator.BestCost += (float)(dwTimeDifference*COEFFICIENT);
////	float range_sqr		= Range*Range;
////
////	// Cycle
////	for (u32 it=0; it<q_stack.size(); it++) {
////		u32 ID = q_stack[it];
////		NodeCompressed*	N = m_nodes_ptr	[ID];
////		u32 L_count = u32(N->links);
////		NodeLink* L_it = (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
////		NodeLink* L_end	= L_it+L_count;
////		for( ; L_it!=L_end; L_it++) {
////			if (bStop)			
////				break;
////			// test node
////			u32 Test = UnpackLink(*L_it);
////			if (q_mark[Test])
////				continue;
////
////			NodeCompressed*	T = m_nodes_ptr[Test];
////
////			float distance_sqr = u_SqrDistance2Node(BasePos,T);
////			if (distance_sqr>range_sqr)	
////				continue;
////
////			// register
////			q_mark[Test]		+= 1;
////			q_stack.push_back	(Test);
////
////			// estimate
////			float cost = Estimator.Estimate(T,distance_sqr,bStop);
////			cost += (float)(dwTimeDifference*1/_sqrt(distance_sqr)*COEFFICIENT);
////			
////			if (cost<Estimator.BestCost) {
////				Estimator.BestCost	= cost;
////				Estimator.BestNode	= Test;
////			}
////		}
////		if (bStop)
////			break;
////	}
////
////	// Clear q_marks
////	{
////		vector<u32>::iterator it	= q_stack.begin();
////		vector<u32>::iterator end	= q_stack.end();
////		for ( ; it!=end; it++)	
////			q_mark[*it] -= 1;
////	}
////
////	Device.Statistic.AI_Range.End();
//}
