////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test_old.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Old path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_a_star_search.h"
#include "level_graph.h"

#define TEST_COUNT 1000

class CAIMapShortestPathNode {
public:
	typedef		CLevelGraph::const_iterator const_iterator;
	float		y1;
	int			x2;
	float		y2;
	int			z2;
	int			x3;
	float		y3;
	int			z3;
	SAIMapData	tData;
	float		m_fYSize2;
	CLevelGraph::CVertex	*m_tpNode;
	float		m_sqr_distance_xz;


	IC CAIMapShortestPathNode(const SAIMapData &tAIMapData)
	{
		tData					= tAIMapData;
		const CLevelGraph::CVertex	&tNode1	= *tData.m_tpAI_Map->vertex(tData.dwFinishNode);
		tData.m_tpAI_Map->unpack_xz(tNode1,x3,z3);
		m_sqr_distance_xz		= _sqr(tData.m_tpAI_Map->header().cell_size());
		y3						= (float)(tNode1.position().y());
		m_fYSize2				= _sqr((float)(tData.m_tpAI_Map->header().factor_y()/32767.0));
	}

	IC void begin(u32 dwNode, const_iterator &tIterator, const_iterator &tEnd)
	{
		tData.m_tpAI_Map->begin	(dwNode,tIterator,tEnd);
		m_tpNode				= tData.m_tpAI_Map->vertex(dwNode);
		const CLevelGraph::CVertex	&tNode0	= *m_tpNode;
		y1						= (float)(tNode0.position().y());
	}

	IC u32 get_value(const_iterator &tIterator)
	{
		return(m_tpNode->link(tIterator));
	}

	IC bool bfCheckIfAccessible(u32 dwNode)
	{
		return(true);
	}

	IC float ffEvaluate(u32 dwStartNode, u32 dwFinishNode, const_iterator &tIterator)
	{
		const CLevelGraph::CVertex	&tNode1 = *tData.m_tpAI_Map->vertex(dwFinishNode);

		y2						= (float)(tNode1.position().y());

		return					(_sqrt(m_fYSize2*_sqr(y2 - y1) + m_sqr_distance_xz));
	}

	IC float ffAnticipate(u32 dwStartNode)
	{
		const CLevelGraph::CVertex	&tNode0 = *tData.m_tpAI_Map->vertex(dwStartNode);
		
		tData.m_tpAI_Map->unpack_xz(tNode0,x2,z2);
		y2						= (float)(tNode0.position().y());

		return					(_sqrt((float)(m_sqr_distance_xz*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}

	IC float ffAnticipate()
	{
		return					(_sqrt((float)(m_sqr_distance_xz*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}
};

class CSearch {
	u32					m_dwAStarStaticCounter;
	SNode				*m_tpHeap;
	SNode				**m_tppHeap;
	SIndexNode			*m_tpIndexes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;
	SAIMapData			tData;

public:
	xr_vector<u32>		tpaNodes;

						CSearch		(const CLevelGraph *tAI_Map)
	{
		tData.m_tpAI_Map		= tAI_Map;
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (tAI_Map->header().vertex_count() + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (tAI_Map->header().vertex_count())*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (tAI_Map->header().vertex_count())*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S3);
		ZeroMemory				(m_tppHeap,S3);
	}

						~CSearch	()
	{
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_free(m_tppHeap);
	}

			void		Find		(u32 dwStart, u32 dwEnd, float &f, u32 &v1)
	{
		tData.dwFinishNode	= dwEnd;
		f = 0.f;
		m_tpMapPath.vfFindOptimalPath(
			m_tppHeap,
			m_tpHeap,
			m_tpIndexes,
			m_dwAStarStaticCounter,
			tData,
			dwStart,
			dwEnd,
			tpaNodes,
			f,
			v1);
	}
};

void path_test_old(LPCSTR caLevelName)
{
	CLevelGraph				*graph			= xr_new<CLevelGraph>				(caLevelName);
	CSearch					*search			= xr_new<CSearch>				(graph);

	xr_vector<u32>			a;
//	a.resize				(ROWS*COLUMNS);
//	for (int i=0, n = ROWS*COLUMNS; i<n; ++i)
//		a[i] = (i/COLUMNS + 1)*(COLUMNS + 2) + i%COLUMNS + 1;

	a.resize				(graph->header().vertex_count());
	for (int i=0, n = (int)a.size(); i<n; ++i)
		a[i] = i;
	
	std::random_shuffle		(a.begin(),a.end());
	Msg						("%d times",_min((int)a.size(),TEST_COUNT));

	u64						start, finish;
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	start					= CPU::GetCycleCount();
	float f;
	u32 v;
	for (int i=0, n=_min((int)a.size(),TEST_COUNT); i<n; i++)
		search->Find		(a[i],a[n - 1 - i],f,v);
	finish					= CPU::GetCycleCount();
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);
	Msg						("%f microseconds per search",float(s64(finish - start))*CPU::cycles2microsec/_min((int)a.size(),TEST_COUNT));

	xr_delete				(graph);
	xr_delete				(search);
}

void path_test_old(CLevelGraph *graph, xr_vector<u32> &path, u32 start, u32 end, float &f, u32 &v1)
{
	CSearch					*search			= xr_new<CSearch>				(graph);

	search->Find			(start,end,f,v1);

	path					= search->tpaNodes;

	xr_delete				(search);
}