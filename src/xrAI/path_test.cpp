////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "path_test.h"

// algorithms
#include "a_star.h"
#include "dijkstra.h"

//	graphs
#include "level_graph.h"
#include "game_graph.h"
#include "test_table.h"

//	pathmanagers
#include "path_manager.h"

// allocators
#include "vertex_allocator_generic.h"
#include "vertex_allocator_fixed.h"

// vertex managers
#include "vertex_manager_generic.h"
#include "vertex_manager_fixed.h"

//		lists
#include "data_storage_bucket_list.h"
#include "data_storage_cheap_list.h"
#include "data_storage_single_linked_list.h"
#include "data_storage_double_linked_list.h"

//		priority queues
#include "boost/f_heap.hpp"
#include "boost/p_heap.hpp"
#include "data_storage_priority_queue.h"
#include "data_storage_binary_heap.h"
#include "data_storage_binary_heap_list.h"
#include "data_storage_multi_binary_heap.h"

extern LPCSTR GAME_LEVEL_GRAPH;

struct SPathParams{
	SPathParams() {};
	SPathParams(u32 x, u32 y) : x(x), y(y) {};
	u32 x,y;
};

template <
	typename _dist_type,
	typename _priority_queue,
	typename _vertex_manager,
	typename _vertex_allocator,
	typename _Graph
>
void test					(const _Graph *graph, const xr_vector<SPathParams> &path_params)
{
	typedef CAStar<_dist_type,_priority_queue,_vertex_manager,_vertex_allocator>									_AStarSearch;
	typedef CPathManager<_Graph,_AStarSearch::CDataStorage,SBaseParameters<_dist_type,u32,u32>,_dist_type,u32,u32>	_PathManager;

	_AStarSearch			*a_star			= xr_new<_AStarSearch>			(graph->header().vertex_count());
	_PathManager			*path_manager	= xr_new<_PathManager>			();

	xr_vector<u32>			path;
	u64						start, finish;
	u32						test_count = path_params.size();

#ifndef DEBUG
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
#endif
	Sleep					(1);
	
	start					= CPU::GetCycleCount();
	
	xr_vector<SPathParams>::const_iterator	I = path_params.begin();
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		path_manager->setup		(
			graph,
			&a_star->data_storage(),
			&path,
			(*I).x,
			(*I).y,
			SBaseParameters<_dist_type,u32,u32>()
		);
		a_star->find		(*path_manager);
	}

	finish					= CPU::GetCycleCount();
	
#ifndef DEBUG
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
#endif

	xr_delete				(a_star);
	xr_delete				(path_manager);

	Msg						("%12.6f (%14.6f) : %s",float(s64(finish - start))*CPU::cycles2milisec/_min((int)path_params.size(),test_count),float(s64(finish - start))*CPU::cycles2microsec,typeid(_AStarSearch::CDataStorage).name());
}

template <
	typename _dist_type,
	typename _priority_queue,
	typename _vertex_manager,
	typename _vertex_allocator,
	typename _Graph
>
void test					(const _Graph *graph, const xr_vector<SPathParams> &path_params, _dist_type min_value, _dist_type max_value)
{
	typedef CAStar<_dist_type,_priority_queue,_vertex_manager,_vertex_allocator>									_AStarSearch;
	typedef CPathManager<_Graph,_AStarSearch::CDataStorage,SBaseParameters<_dist_type,u32,u32>,_dist_type,u32,u32>	_PathManager;

	_AStarSearch			*a_star			= xr_new<_AStarSearch>			(graph->header().vertex_count(),min_value,max_value);
	_PathManager			*path_manager	= xr_new<_PathManager>			();

	xr_vector<u32>			path;
	u64						start, finish;
	u32						test_count = path_params.size();
	
#ifndef DEBUG
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
#endif
	Sleep					(1);
	
	start					= CPU::GetCycleCount();
	
	xr_vector<SPathParams>::const_iterator	I = path_params.begin();
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		path_manager->setup		(
			graph,
			&a_star->data_storage(),
			&path,
			(*I).x,
			(*I).y,
			SBaseParameters<_dist_type,u32,u32>()
		);
		a_star->find		(*path_manager);
	}

	finish					= CPU::GetCycleCount();
	
#ifndef DEBUG
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
#endif

	xr_delete				(a_star);
	xr_delete				(path_manager);

	Msg						("%12.6f (%14.6f) : %s",float(s64(finish - start))*CPU::cycles2milisec/_min((int)path_params.size(),test_count),float(s64(finish - start))*CPU::cycles2microsec,typeid(_AStarSearch::CDataStorage).name());
}

template <
	typename _Graph
>
void init_search			(LPCSTR caLevelName, _Graph *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<_Graph>(caLevelName);
	u32						n = graph->header().vertex_count()();
	path_params.resize		(graph->header().vertex_count()());
	xr_vector<SPathParams>::const_iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		(*I).x				= I - B;
		(*I).y				= E - I;
	}
	std::random_shuffle		(path_params.begin(),path_params.end());
}

#define TEST_DIST1	150.f
#define TEST_DIST2	100.f

template <>
void init_search<CLevelGraph>	(LPCSTR caLevelName, CLevelNavigationGraph *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<CLevelGraph>(caLevelName);

	if (bRandom) {
		u32					n = graph->header().vertex_count();
		path_params.resize	(n - 2);
		xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
		xr_vector<SPathParams>::iterator	E = path_params.end();
		for ( ; I != E; ++I) {
			(*I).x			= ::Random.randI(1,n);
			(*I).y			= ::Random.randI(1,n);
		}
	}
//	else
//		for (int i=1, n=graph->header().vertex_count()(); i<n; ++i) {
//			for (int j=i + 1; j<n; j++)
//				if ((graph->get_edge_weight(i,j) > TEST_DIST2) && (graph->get_edge_weight(i,j) <= TEST_DIST1)) {
//					path_params.push_back(SPathParams(i,j));
//					if (path_params.size() >= TEST_COUNT)
//						break;
//				}
//				if (path_params.size() >= TEST_COUNT)
//					break;
//		}

	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <>
void init_search<CGameGraph>(LPCSTR caLevelName, CGameGraph *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	string256				fName;
	strconcat				(fName,caLevelName,GAME_LEVEL_GRAPH);
	graph					= xr_new<CGameGraph>(fName);
	u32						n = graph->header().vertex_count();
	path_params.resize		(graph->header().vertex_count());
	
	xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		(*I).x				= ::Random.randI(0,n);
		(*I).y				= ::Random.randI(0,n);
	}
	
	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <typename _dist_type, u32 rows, u32 columns>
void init_search(LPCSTR caLevelName, CTestTable<_dist_type,rows,columns> *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<CTestTable<_dist_type,rows,columns> >(caLevelName);
	u32						n = rows*columns;
	path_params.resize		(rows*columns);
	
	xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::iterator	E = path_params.end();
	for (int i; I != E; ++I) {
		i					= ::Random.randI(0,n);
		(*I).x				= (i/columns + 1)*(columns + 2) + i%columns + 1;
		i					= ::Random.randI(0,n);
		(*I).y				= (i/columns + 1)*(columns + 2) + i%columns + 1;
	}
	
	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <
	typename _Graph,
	typename _dist_type
>
void test_all				(LPCSTR caLevelName, u32 test_count, _dist_type min_value, _dist_type max_value)
{
	xr_vector<SPathParams>	path_params;
	_Graph					*graph;
	
	Msg						("\nTest %s",typeid(_Graph).name());
	
	init_search				(caLevelName,graph,path_params,true);

	path_params.resize		(_min(path_params.size(),test_count));
				
	typedef CVertexManagerFixed<u32,u32,8>				CVertexManager;
	typedef CVertexAllocatorFixed<1048576>				CVertexAllocator;
//	typedef CDataStorageAllocator CVertexAllocator;

	test<_dist_type,CDataStorageSingleLinkedList<false>,				CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageSingleLinkedList<true>,					CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageDoubleLinkedList<false>,				CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageDoubleLinkedList<true>,					CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageCheapList<32,true,true>,				CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageBucketList<u32,u32,8*1024,false>,		CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageBinaryHeap,								CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageBinaryHeapList<4>,						CVertexManager,CVertexAllocator>	(graph,path_params);
	test<_dist_type,CDataStorageMultiBinaryHeap<4>,						CVertexManager,CVertexAllocator>	(graph,path_params);
//	test<_dist_type,CPriorityQueue<boost::fibonacci_heap>,	CVertexManager,CVertexAllocator>	(graph,path_params);
//	test<_dist_type,CPriorityQueue<boost::pairing_heap>,		CVertexManager,CVertexAllocator>	(graph,path_params);

	xr_delete				(graph);
}

#define TEST_COUNT 1000

#include "graph_engine.h"

void path_test				(LPCSTR caLevelName)
{
	test_all<CLevelGraph>				(caLevelName,TEST_COUNT,float(0),float(2000));
	test_all<CGameGraph>				(caLevelName,TEST_COUNT,float(0),float(2000));
	test_all<CTestTable<u32,30,30> >	(caLevelName,TEST_COUNT,u32(0),u32(60));
	test_all<CTestTable<u32,300,300> >	(caLevelName,TEST_COUNT,u32(0),u32(600));
	test_all<CTestTable<u32,900,900> >	(caLevelName,TEST_COUNT,u32(0),u32(1800));
//	CLevelGraph			*level_graph	= xr_new<CLevelGraph>(caLevelName);
//	CGraphEngine		*graph_engine	= xr_new<CGraphEngine>(level_graph->header().vertex_count());
//	graph_engine->search(*level_graph,0,160,0,CGraphEngine::CBaseParameters());
//	xr_delete			(level_graph);
//	xr_delete			(graph_engine);
}
