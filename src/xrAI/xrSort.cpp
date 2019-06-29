////////////////////////////////////////////////////////////////////////////
//	Module 		: xrSort.cpp
//	Created 	: 11.12.2002
//  Modified 	: 11.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Sort routines
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrGraph.h"
#include "xrSort.h"

typedef struct tagSQuickStructure {
	u32	si;
	u32	sj;
} SQuickStructure;

#define QUICK_SORT_STACK_SIZE			1024
#define QUICK_SORT_THRESHOLD			17
#define QPUSH(i,j)						tpStackPointer->si = i, (tpStackPointer++)->sj = j
#define QPOP(i,j)						i = (--tpStackPointer)->si, j = tpStackPointer->sj
#define QSWAP(i,j) {\
	u32 uiTemp = *i;\
	*i = *j;\
	*j = uiTemp;\
}

void vfSimpleInsertionSort(CGameGraph::CEdge *tpaEdges, u32 *uiaSortOrder, int i,int j)
{
	for (int k=1; k < j - i + 1; k++) {
		for (u32 *uipPointer = uiaSortOrder + i + k, *uipStop = uiaSortOrder + i; uipPointer > uipStop; uipPointer--) {
			if (tpaEdges[*(uipPointer - 1)].distance() > tpaEdges[*uipPointer].distance()) {
				u32 uiTemp = *uipPointer;
				*uipPointer = *(uipPointer - 1);
				*(uipPointer - 1)= uiTemp;
			}
			else
				break;
		}
	}
}

__forceinline void vfSwapVectors(u32 *i, u32 *j, int n)
{
	while (n-- > 0) {
		QSWAP(i,j);
		i++;
		j++;
	}
}

void vfQuickSortEdges(CGameGraph::CEdge *tpaEdges, u32 *uiaSortOrder, int n)
{
	SQuickStructure tpStack[QUICK_SORT_STACK_SIZE], *tpStackPointer = tpStack;
	int iStartI;
	QPUSH(0,n);
	while (tpStackPointer > tpStack) {
		QPOP(iStartI,n);
		for (;;) {
			if (n < 2)
				break;
			if ((n < QUICK_SORT_THRESHOLD) || (tpStackPointer - tpStack>= QUICK_SORT_STACK_SIZE - 1)) {
				vfSimpleInsertionSort(tpaEdges,uiaSortOrder,iStartI,iStartI + n - 1);
				break;
			}
			float r, v;
			u32 *a, *b, *c, *d;
			u32 *uiaArray = uiaSortOrder + iStartI;
			v = tpaEdges[*uiaArray].distance();
			a = b = uiaArray + 1;
			c = d = uiaArray + n - 1;
			for (;;) {
				while ((b <= c) && ((r = tpaEdges[*b].distance() - v) <= 0)) {
					if (r == 0) {
						QSWAP(a,b);
						a++;
					}
					b++;
				}
				while ((b <= c) && ((r = tpaEdges[*c].distance() - v) >= 0)) {
					if (r == 0) {
						QSWAP(c,d);
						d--;
					}
					c--;
				}
				if (b > c)
					break;
				QSWAP(b,c);
				b++;
				c--;
			}
			if (d < a)
				break;
			int rr;
			rr = _min(int(a - uiaArray),int(b - a));
			vfSwapVectors(uiaArray,b - rr,rr);
			rr = _min(int(d - c),int(n - (d - uiaArray) - 1));
			vfSwapVectors(b,uiaArray + n - rr,rr);
			rr = int(b - a);
			QPUSH(iStartI,rr);
			rr = int(d - c);
			iStartI += n - rr;
			n = rr;
		}
	}
}

