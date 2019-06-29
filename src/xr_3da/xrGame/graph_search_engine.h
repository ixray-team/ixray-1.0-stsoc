////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_search_engine.h
//	Created 	: 21.03.2002
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph search engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "a_star.h"
#include "data_storage_priority_queue.h"
#include "data_storage_heap.h"
#include "data_storage_list.h"
#include "boost\\f_heap.hpp"
#include "boost\\p_heap.hpp"
#include "path_manager.h"

class CGraphSearchEngine {
protected:
	// distance type definition
	typedef float _dist_type;
	typedef u32	  _index_type;
	typedef u32	  _iteration_type;
public:
	typedef PathManagers::SBaseParameters<_dist_type,_index_type,_iteration_type>			CBaseParameters;
	typedef PathManagers::SFlooder<_dist_type,_index_type,_iteration_type>					CFlooder;
	typedef PathManagers::SObstacleParams<_dist_type,_index_type,_iteration_type>			CObstacleParams;
	typedef PathManagers::SObstaclesLightCover<_dist_type,_index_type,_iteration_type>		CObstaclesLightCover;
	typedef PathManagers::SObstaclesLightCoverEnemy<_dist_type,_index_type,_iteration_type> CObstaclesLightCoverEnemy;
	typedef PathManagers::SPosition<_dist_type,_index_type,_iteration_type>					CPositionParameters;
	typedef PathManagers::SStraightLineParams<_dist_type,_index_type,_iteration_type>		CStraightLineParams;
protected:

	// data storage type definition
//	typedef CDataStorageUL				<							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageDLUL			<							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageSL				<							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageDLSL			<							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageBinaryHeap		<							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageBinaryHeapList	<4,							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageMultiBinaryHeap	<4,							_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStorageCheapList		<32,true,true,				_dist_type,_index_type,u32,true,24,8> CDataStorage;
	typedef CDataStorageBucketList		<8*1024,false,				_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStoragePriorityQueue	<boost::fibonacci_heap,		_dist_type,_index_type,u32,true,24,8> CDataStorage;
//	typedef CDataStoragePriorityQueue	<boost::pairing_heap,		_dist_type,_index_type,u32,true,24,8> CDataStorage;
	
	CDataStorage					*m_data_storage;

public:

					CGraphSearchEngine		(u32 max_node_count)
	{
		m_data_storage		= xr_new<CDataStorage>(max_node_count,_dist_type(0),_dist_type(2000));
	}

	virtual			~CGraphSearchEngine		()
	{
		xr_delete			(m_data_storage);
	}

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	build_path				(
				const _Graph			&graph, 
				u32						start_node, 
				u32						dest_node, 
				xr_vector<u32>			*node_path,
				const _Parameters		&parameters
			)
	{
		typedef CPathManager<_Graph, CDataStorage, _Parameters, _dist_type,_index_type,u32>	CPathManagerGeneric;
		typedef CAStar<CDataStorage,CPathManagerGeneric,_iteration_type,_dist_type> CAStarGeneric;

		CPathManagerGeneric			path_manager;
		CAStarGeneric				a_star;

		VERIFY						(graph.header().vertex_count() <= m_data_storage->get_max_node_count());

		path_manager.setup			(
			&graph,
			m_data_storage,
			node_path,
			start_node,
			dest_node,
			parameters
		);
		return						(a_star.find(*m_data_storage,path_manager));
	}

	template <
		typename _Graph,
		typename _Parameters
	>
	IC		bool	build_path				(
				const _Graph			&graph, 
				u32						start_node, 
				u32						dest_node, 
				xr_vector<u32>			*node_path,
				_Parameters				&parameters
			)
	{
		typedef CPathManager<_Graph, CDataStorage, _Parameters, _dist_type,_index_type,u32>	CPathManagerGeneric;
		typedef CAStar<CDataStorage,CPathManagerGeneric,_iteration_type,_dist_type> CAStarGeneric;

		CPathManagerGeneric			path_manager;
		CAStarGeneric				a_star;

		VERIFY						(graph.header().vertex_count() <= m_data_storage->get_max_node_count());

		path_manager.setup			(
			&graph,
			m_data_storage,
			node_path,
			start_node,
			dest_node,
			parameters
		);
		return						(a_star.find(*m_data_storage,path_manager));
	}
};