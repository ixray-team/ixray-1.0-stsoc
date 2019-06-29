////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_search.h
//	Created 	: 21.03.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding optimal path from source node to destnation one
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_VALUE 100000000.f

class CLevelGraph;

typedef struct tagSAIMapData {
	u32				dwFinishNode;
	const CLevelGraph	*m_tpAI_Map;
} SAIMapData;

#pragma pack(push,4)
typedef struct tagSNode {
	u32			ucOpenCloseMask:8;
	u32			iIndex:24;
	float		f;
	float		g;
	float		h;
	tagSNode	*tpBack;
} SNode;

typedef struct tagSIndexNode {
	SNode		*tpNode;
	u32			dwTime;
} SIndexNode;
#pragma pack(pop)

class CComparePredicate {
public:
	__forceinline bool operator()(const SNode *tpNode1, const SNode *tpNode2) const
	{
		return(tpNode1->f > tpNode2->f);
	};
};

template<class CTemplateNode, class SData> class CAStarSearch {
public:
	void vfFindOptimalPath(
			SNode		**m_tppHeap,
			SNode		*tpHeap,
			SIndexNode	*tpIndexes,
			u32			&dwAStarStaticCounter,
			SData		&tData,
			u32			dwStartNode, 
			u32			dwGoalNode, 
			float		&fValue, 
			float		fMaxValue, 
			Fvector		tStartPosition, 
			Fvector		tFinishPosition, 
			xr_vector<u32> &tpaNodes,
			u32			&dwMaxCount)
	{
		// initialization
		dwAStarStaticCounter++;

		u32					dwHeap = 0;
		CTemplateNode		tTemplateNode(tData);

		SNode				*tpTemp = tpIndexes[dwStartNode].tpNode = tpHeap + dwHeap++,
							*tpTemp1,
							*tpTemp2,
							*tpBestNode,
							**tppHeapStart = m_tppHeap + 0,
							**tppHeapEnd = m_tppHeap + 1;
		
		tpTemp->tpBack = 0;
		
		tpIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

		tpTemp->iIndex = dwStartNode;
		tpTemp->g = 0.0;
		tpTemp->h = tTemplateNode.ffAnticipate(dwStartNode);
		tpTemp->ucOpenCloseMask = 1;
		tpTemp->f = tpTemp->g + tpTemp->h;
		*tppHeapStart = tpTemp;
		
		for (;;) {
			
			// finding the node being estimated as the cheapest among the opened ones
			tpBestNode = *tppHeapStart;
			
			// checking if the distance is not too large
			if (tpBestNode->f >= fMaxValue) {
				fValue = MAX_VALUE;
				return;
			}

			// check if that node is our goal
			int iBestIndex = tpBestNode->iIndex;
			if (iBestIndex == (int)dwGoalNode) {
				fValue = tpBestNode->g;

				tpTemp1 = tpBestNode;
				tpTemp = tpTemp1->tpBack;
				for (u32 i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

				tpaNodes.resize(i);

				tpTemp1 = tpBestNode;
				tpaNodes[--i] = tpBestNode->iIndex;
				tpTemp = tpTemp1->tpBack;

				{
					xr_vector<u32>::reverse_iterator	I = tpaNodes.rbegin();
					xr_vector<u32>::reverse_iterator	E = tpaNodes.rend();
					for (I++ ; tpTemp; tpTemp = tpTemp->tpBack, I++)
						*I = tpTemp->iIndex;
				}

				float fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;
				Fvector tPosition = tStartPosition;
				
				xr_vector<u32>::iterator	I = tpaNodes.begin();
				xr_vector<u32>::iterator	E = tpaNodes.end();
				u32 dwNode = *I;
				for (I++; I != E; I++) {
					fDirectDistance = tTemplateNode.tData.m_tpAI_Map->check_position_in_direction(dwNode,tPosition,tTemplateNode.tData.m_tpAI_Map->vertex_position(*I),fMaxValue);
					if (fDirectDistance == MAX_VALUE) {
						if (fLastDirectDistance == 0) {
							fCumulativeDistance += tTemplateNode.tData.m_tpAI_Map->distance(dwNode,*I);
							dwNode = *I;
						}
						else {
							fCumulativeDistance += fLastDirectDistance;
							fLastDirectDistance = 0;
							dwNode = *--I;
						}
						tPosition = tTemplateNode.tData.m_tpAI_Map->vertex_position(dwNode);
					}
					else 
						fLastDirectDistance = fDirectDistance;
					if (fCumulativeDistance + fLastDirectDistance >= fMaxValue) {
						fValue = MAX_VALUE;
						return;
					}
				}

				fDirectDistance = tTemplateNode.tData.m_tpAI_Map->check_position_in_direction(dwNode,tPosition,tFinishPosition,fMaxValue);
				if (fDirectDistance == MAX_VALUE)
					fValue = fCumulativeDistance + fLastDirectDistance + tFinishPosition.distance_to(tTemplateNode.tData.m_tpAI_Map->vertex_position(tpaNodes[tpaNodes.size() - 1]));
				else
					fValue = fCumulativeDistance + fDirectDistance;
				return;
			}
			
			// remove that node from the opened list and put that node to the closed list
			tpBestNode->ucOpenCloseMask = 0;
			std::pop_heap(tppHeapStart,tppHeapEnd--,CComparePredicate());

			// iterating on children/neighbours
			CTemplateNode::const_iterator tIterator;
			CTemplateNode::const_iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				if (tpIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					// check if this node is already in the opened list
					tpTemp = tpIndexes[iNodeIndex].tpNode;
					if (tpTemp->ucOpenCloseMask) {
						// initialize node
						float dG = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);
						if (tpTemp->g > dG) {
							tpTemp->g = dG;
							tpTemp->f = tpTemp->g + tpTemp->h;
							tpTemp->tpBack = tpBestNode;
							for (SNode **tpIndex = tppHeapStart; *tpIndex != tpTemp; tpIndex++);
							std::push_heap(tppHeapStart,tpIndex + 1,CComparePredicate());
							continue;
						}
						continue;
					}
					continue;
				}
				else {
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = tTemplateNode.ffAnticipate();
					tpTemp2->f = tpTemp2->g + tpTemp2->h;
					
					tpTemp2->ucOpenCloseMask = 1;
					
					*tppHeapEnd = tpTemp2;
					std::push_heap(tppHeapStart,++tppHeapEnd,CComparePredicate());
				}
			}
			if (dwHeap > dwMaxCount)
				break;
		}
		fValue = MAX_VALUE;
	}

	void vfFindOptimalPath(
			SNode		**m_tppHeap,
			SNode		*tpHeap,
			SIndexNode	*tpIndexes,
			u32			&dwAStarStaticCounter,
			SData		&tData,
			u32			dwStartNode, 
			u32			dwGoalNode, 
			xr_vector<u32> &tpaNodes,
			float &f,
			u32 &v1)
	{
		// initialization
		dwAStarStaticCounter++;

		u32					dwHeap = 0;
		CTemplateNode		tTemplateNode(tData);

		SNode				*tpTemp = tpIndexes[dwStartNode].tpNode = tpHeap + dwHeap++,
							*tpTemp1,
							*tpTemp2,
							*tpBestNode,
							**tppHeapStart = m_tppHeap + 0,
							**tppHeapEnd = m_tppHeap + 1;
		
//		Memory.mem_fill(tpTemp,0,sizeof(SNode));
		tpTemp->tpBack = 0;
		
		tpIndexes[dwStartNode].dwTime = dwAStarStaticCounter;

		tpTemp->iIndex = dwStartNode;
		tpTemp->g = 0.0;
		tpTemp->h = tTemplateNode.ffAnticipate(dwStartNode);
		tpTemp->ucOpenCloseMask = 1;
		tpTemp->f = tpTemp->g + tpTemp->h;
		*tppHeapStart = tpTemp;
		
		for (;;) {
			
			// finding the node being estimated as the cheapest among the opened ones
			tpBestNode = *tppHeapStart;
			
			// checking if the distance is not too large
			// check if that node is our goal
			int iBestIndex = tpBestNode->iIndex;
			if (iBestIndex == (int)dwGoalNode) {
				f = tpBestNode->f;
				v1 = dwHeap;
				tpTemp1 = tpBestNode;
				tpTemp = tpTemp1->tpBack;
				for (u32 i=1; tpTemp; tpTemp1 = tpTemp, tpTemp = tpTemp->tpBack, i++) ;

				tpaNodes.resize(i);

				tpTemp1 = tpBestNode;
				tpaNodes[--i] = tpBestNode->iIndex;
				tpTemp = tpTemp1->tpBack;

				{
					xr_vector<u32>::reverse_iterator	I = tpaNodes.rbegin();
					xr_vector<u32>::reverse_iterator	E = tpaNodes.rend();
					for (I++ ; tpTemp; tpTemp = tpTemp->tpBack, I++)
						*I = tpTemp->iIndex;
				}

				return;
			}
			
			// remove that node from the opened list and put that node to the closed list
			tpBestNode->ucOpenCloseMask = 0;
			std::pop_heap(tppHeapStart,tppHeapEnd--,CComparePredicate());

			// iterating on children/neighbours
			CTemplateNode::const_iterator tIterator;
			CTemplateNode::const_iterator tEnd;
			tTemplateNode.begin(iBestIndex,tIterator,tEnd);
			for (  ; tIterator != tEnd; tIterator++) {
				int iNodeIndex = tTemplateNode.get_value(tIterator);
				if (tpIndexes[iNodeIndex].dwTime == dwAStarStaticCounter) {
					// check if this node is already in the opened list
					tpTemp = tpIndexes[iNodeIndex].tpNode;
					if (tpTemp->ucOpenCloseMask) {
						// initialize node
						float dG = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);
						if (tpTemp->g > dG) {
							tpTemp->g = dG;
							tpTemp->f = tpTemp->g + tpTemp->h;
							tpTemp->tpBack = tpBestNode;
							for (SNode **tpIndex = tppHeapStart; *tpIndex != tpTemp; tpIndex++);
							std::push_heap(tppHeapStart,tpIndex + 1,CComparePredicate());
							continue;
						}
						continue;
					}
					continue;
				}
				else {
					tpTemp2 = tpIndexes[iNodeIndex].tpNode = tpHeap + dwHeap++;
					tpIndexes[iNodeIndex].dwTime = dwAStarStaticCounter;

					tpTemp2->iIndex = iNodeIndex;
					tpTemp2->tpBack = tpBestNode;
					tpTemp2->g = tpBestNode->g + tTemplateNode.ffEvaluate(iBestIndex,iNodeIndex,tIterator);

					// put that node to the opened list if wasn't found there and in the closed one
					tpTemp2->h = tTemplateNode.ffAnticipate();
					tpTemp2->f = tpTemp2->g + tpTemp2->h;
					
					tpTemp2->ucOpenCloseMask = 1;
					
					*tppHeapEnd = tpTemp2;
					std::push_heap(tppHeapStart,++tppHeapEnd,CComparePredicate());
				}
			}
		}
	}
};