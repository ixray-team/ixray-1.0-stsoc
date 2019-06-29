////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "game_graph.h"

typedef struct tagSDynamicGraphVertex {
	Fvector						tLocalPoint;
	Fvector						tGlobalPoint;
	u32							tNodeID;
	u8							tVertexTypes[GameGraph::LOCATION_TYPE_COUNT];
	u32							tLevelID;
	u32							tNeighbourCount;
	u32							tDeathPointCount;
	u32							dwPointOffset;
	CGameGraph::CEdge			*tpaEdges;
} SDynamicGraphVertex;

#define MAX_DISTANCE_TO_CONNECT		4096.f

DEFINE_VECTOR(SDynamicGraphVertex,		GRAPH_VERTEX_VECTOR,	GRAPH_VERTEX_IT);
DEFINE_VECTOR(CGameGraph::CEdge,	GRAPH_EDGE_VECTOR,		GRAPH_EDGE_IT);

extern xr_vector<SDynamicGraphVertex>	tpaGraph;		// graph
extern CGameGraph::CHeader				tGraphHeader;

void xrBuildGraph(LPCSTR name);
void xrMergeSpawns();