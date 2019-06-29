////////////////////////////////////////////////////////////////////////////
//	Module 		: connectivity_test.cpp
//	Created 	: 09.08.2005
//  Modified 	: 09.08.2005
//	Author		: Dmitriy Iassenev
//	Description : connectivity test
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
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

#include "graph_abstract.h"

#define index(i,j) (((i)+1)*(M+2)+(j)+1)

struct sector_vertex {
	u32		m_min_vertex_id;
	u32		m_max_vertex_id;

	IC		sector_vertex	()
	{
		m_min_vertex_id	= u32(-1);
		m_max_vertex_id	= u32(-1);
	}

	IC		sector_vertex	(u32 min, u32 max)
	{
		m_min_vertex_id	= min;
		m_max_vertex_id	= max;
	}
};

void connectivity_test	(LPCSTR level_name)
{
	const u32				N = 1024;
	const u32				M = 1024;

	typedef u32										_dist_type;
	typedef CVertexManagerFixed<u32,u32,8>			CVertexManager;
	typedef CVertexAllocatorFixed<(N+2)*(M+2)>	CVertexAllocator;
	typedef CDataStorageCheapList<32,true,true>		CPriorityQueue;
	typedef CAStar<
		_dist_type,
		CPriorityQueue,
		CVertexManager,
		CVertexAllocator
	>												CAlgorithm;
	typedef CTestTable<_dist_type,N,M>				CGraph;
	typedef CPathManager<CGraph,CAlgorithm::CDataStorage,SBaseParameters<_dist_type,u32,u32>,_dist_type,u32,u32>	CPathManager;
	typedef CGraphAbstract<sector_vertex>			CSectorGraph;

	CGraph					*graph  =xr_new<CGraph>("connectivity_test_graph");
	CSectorGraph			sectors;
	CAlgorithm				*algorithm = xr_new<CAlgorithm>((N+2)*(M+2));
	CPathManager			path_manager;
	CTimer					timer;
	u32						start_node, dest_node;
	xr_vector<u32>			path;
	start_node				= index(0,0);
	dest_node				= index(N-1,M-1);
	sector_vertex			s(index(0,0),index(N-1,M-1));
	sectors.add_vertex		(s,0);

//	enable					(*graph,sectors,);

//	graph->enable			(index(N-2,M-1),false);
//	graph->enable			(index(N-1,M-2),false);

	path_manager.setup		(
		graph,
		&algorithm->data_storage(),
		0,//&path,
		start_node,
		dest_node,
		SBaseParameters<_dist_type,u32,u32>()
	);

	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

	Sleep					(1);

	timer.Start				();

	bool					result = algorithm->find(path_manager);

	Msg						("time : %12.6f",timer.GetElapsed_ms());
	Msg						("search %s, %d vertices searched",result ? "succeded" : "failed",algorithm->data_storage().get_visited_node_count());

	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	xr_delete				(graph);
	xr_delete				(algorithm);
}
