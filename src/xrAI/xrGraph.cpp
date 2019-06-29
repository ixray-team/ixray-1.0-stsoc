////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.cpp
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "net_utils.h"
#include "levelgamedef.h"
#include "xrLevel.h"
#include "xrThread.h"
#include "xr_ini.h"
#include "xrAI.h"
#include "ai_nodes.h"
#include "xrGraph.h"
#include "xrSort.h"
#include "xrServer_Objects_ALife_All.h"
#include "factory_api.h"
#include "xrCrossTable.h"
#include "guid_generator.h"

using namespace ALife;

#define THREAD_COUNT				6

#define GET_INDEX(N,K)				iFloor((2*N - 1 - _sqrt((2*N - 1)*(2*N - 1) - 4*float(N)*(N - 1)/float(K)))/2.f)

#define START_THREADS(size,ThreadClass) {\
	u32	stride	= size/THREAD_COUNT;\
	u32	last	= size - stride*(THREAD_COUNT - 1);\
	for (u32 thID=0; thID<THREAD_COUNT; thID++)\
		tThreadManager.start(xr_new<ThreadClass>(thID,thID*stride,thID*stride+((thID==(THREAD_COUNT - 1))?last:stride),tpAI_Map));\
}

typedef struct tagRPoint {
	Fvector	P;
	Fvector A;
} RPoint;

CGameGraph::CEdge				*tpaEdges;		// graph edges
CGameGraph::CEdge				*tpaFullEdges;	// graph edges
xr_vector<SDynamicGraphVertex>	tpaGraph;		// graph
xr_stack<u32>					dwaStack;		// stack
u32								*dwaSortOrder;  // edge sort order
u32								*dwaEdgeOwner;  // edge owners

void vfLoadGraphPoints(LPCSTR name)
{
	string256								fName;
	strconcat								(fName,name,"level.spawn");
	IReader									*F = FS.r_open(fName);
	u32										id;
	IReader									*O = F->open_chunk_iterator(id);
	for (int i=0; O; O = F->open_chunk_iterator(id,O))	{
		NET_Packet							P;
		P.B.count							= O->length();
		O->r								(P.B.data,P.B.count);
		u16									ID;
		P.r_begin							(ID);
		R_ASSERT							(M_SPAWN==ID);
		P.r_stringZ							(fName);
		CSE_Abstract						*E = F_entity_Create(fName);
		R_ASSERT2							(E,"Can't create entity.");
		E->Spawn_Read						(P);
		CSE_ALifeGraphPoint						*tpGraphPoint = smart_cast<CSE_ALifeGraphPoint*>(E);
		if (tpGraphPoint) {
			SDynamicGraphVertex					tDynamicGraphVertex;
			tDynamicGraphVertex.tLocalPoint		= tpGraphPoint->o_Position;
			tDynamicGraphVertex.tGlobalPoint	= tDynamicGraphVertex.tLocalPoint;
			tDynamicGraphVertex.tNodeID			= 0;
			tDynamicGraphVertex.tNeighbourCount	= 0;
			Memory.mem_copy						(tDynamicGraphVertex.tVertexTypes,tpGraphPoint->m_tLocations,GameGraph::LOCATION_TYPE_COUNT*sizeof(GameGraph::_LOCATION_ID));
			tDynamicGraphVertex.tLevelID		= 0;
			tDynamicGraphVertex.tpaEdges		= 0;
			tDynamicGraphVertex.tDeathPointCount = 0;
			tDynamicGraphVertex.dwPointOffset	= 0;
			tpaGraph.push_back					(tDynamicGraphVertex);
			i++;
		}
		F_entity_Destroy					(E);
		
		if (i && ((i % 100) == 0))
			Status							("Vertices read : %d",i);
	}
	FS.r_close								(F);
	R_ASSERT2								(!tpaGraph.empty(),"There are no graph points!");
	Status									("Vertices read : %d",i);
}

void vfRemoveDuplicateGraphPoints(u32 &dwVertexCount)
{
	Progress(0.0f);
	for (int i=0, k=0, N = (int)tpaGraph.size(); i<N; i++)
		for (int j=i+1; j<N; j++)
			if (tpaGraph[i].tLocalPoint.distance_to(tpaGraph[j].tLocalPoint) < EPS_L) {
				tpaGraph.erase(tpaGraph.begin() + j);
				N--;
				j--;
				k++;
			}

	Progress(1.0f);
	dwVertexCount	-= k;
	Msg("%d vertices has been removed",k);
	Msg("%d vertices are valid",dwVertexCount);
}

bool bfCheckForGraphConnectivity(CLevelGraph *tpAI_Map)
{
	if (!tpaGraph.size())
		return				(true);

	while (dwaStack.size())
		dwaStack.pop		();
	dwaStack.push			(0);
	tpAI_Map->q_mark_bit.assign(tpAI_Map->header().vertex_count(),false);
	tpAI_Map->q_mark_bit[0] = true;
	while (!dwaStack.empty()) {
		u32					dwCurrentVertex = dwaStack.top();
		dwaStack.pop		();
		SDynamicGraphVertex &tDynamicGraphVertex = tpaGraph[dwCurrentVertex];
		for (int i=0; i<(int)tDynamicGraphVertex.tNeighbourCount; i++)
			if (!tpAI_Map->q_mark_bit[tDynamicGraphVertex.tpaEdges[i].vertex_id()]) {
				dwaStack.push(tDynamicGraphVertex.tpaEdges[i].vertex_id());
				tpAI_Map->q_mark_bit[tDynamicGraphVertex.tpaEdges[i].vertex_id()] = true;
			}
	}
	bool		ok = true;
	for (int i=0; i<(int)tpaGraph.size(); i++)
		if (!tpAI_Map->q_mark_bit[i]) {
			Msg				("Vertex [%f][%f][%f] is not connected!",VPUSH(tpaGraph[i].tLocalPoint));
			ok				= false;
		}
	
	tpAI_Map->q_mark_bit.assign(tpAI_Map->header().vertex_count(),false);
	return					(ok);
}

u32 dwfErasePoints()
{
	u32 dwPointsWONodes = 0;
	Progress(0.0f);
	for (int i=0; i<(int)tpaGraph.size(); 	Progress(float(++i)/tpaGraph.size()))
		if (int(tpaGraph[i].tNodeID) <= 0) {
			Msg("Point %3d [%7.2f,%7.2f,%7.2f] has no corresponding node",i + dwPointsWONodes++,tpaGraph[i].tLocalPoint.x,tpaGraph[i].tLocalPoint.y,tpaGraph[i].tLocalPoint.z);
			tpaGraph.erase(tpaGraph.begin() + i--);
		}
	Progress(1.0f);
	return(dwPointsWONodes);
}

void vfAllocateMemory()
{
	Progress(0.0f);
	int N = tpaGraph.size();
	tpaFullEdges = (CGameGraph::CEdge *)xr_malloc(N*(N - 1)*sizeof(CGameGraph::CEdge));
	CGameGraph::CEdge	*tpPointer = tpaFullEdges;
	for (int i=0; i<N; tpPointer += (N - 1), i++)
		tpaGraph[i].tpaEdges = tpPointer;
	Msg("* %d memory allocated",N*(N - 1)*sizeof(CGameGraph::CEdge));
	Progress(1.0f);
}

void vfPreprocessEdges(u32 dwEdgeCount)
{
	Progress(0.0f);
	CGameGraph::CEdge *tpPointer = tpaEdges = (CGameGraph::CEdge *)xr_malloc(dwEdgeCount*sizeof(CGameGraph::CEdge));
	dwaSortOrder = (u32 *)xr_malloc(dwEdgeCount*sizeof(u32));
	dwaEdgeOwner = (u32 *)xr_malloc(dwEdgeCount*sizeof(u32));
	for (int i=0, j=0; i<(int)tpaGraph.size(); i++) {
		SDynamicGraphVertex &tDynamicGraphVertex = tpaGraph[i]; 
		Memory.mem_copy(tpPointer,tDynamicGraphVertex.tpaEdges,tDynamicGraphVertex.tNeighbourCount*sizeof(CGameGraph::CEdge));
		tDynamicGraphVertex.tpaEdges = tpPointer;
		tpPointer += tDynamicGraphVertex.tNeighbourCount;
		for (int k=0; k<(int)tDynamicGraphVertex.tNeighbourCount; k++, j++) {
			dwaSortOrder[j] = j;
			dwaEdgeOwner[j] = i;
		}
	}
	xr_free(tpaFullEdges);
	Progress(1.0f);
}

void vfOptimizeGraph(u32 dwEdgeCount, CLevelGraph *tpAI_Map)
{
	tpAI_Map->q_mark_bit.assign(_max(dwEdgeCount,tpAI_Map->header().vertex_count()),false);
	Progress(0.0f);
	for (int i=dwEdgeCount - 1; i>=0; Progress((float(dwEdgeCount) - (i--))/dwEdgeCount)) {
		if (tpAI_Map->q_mark_bit[dwaSortOrder[i]])
			continue;
		u32 dwVertex0 = dwaEdgeOwner[dwaSortOrder[i]];
		u32 dwVertex1 = tpaEdges[dwaSortOrder[i]].vertex_id();
		SDynamicGraphVertex &tVertex0 = tpaGraph[dwVertex0];
		SDynamicGraphVertex &tVertex1 = tpaGraph[dwVertex1];
		bool bOk = true;
		for (int i0=0; (i0<(int)tVertex0.tNeighbourCount) && bOk; i0++) {
			if (tpAI_Map->q_mark_bit[tVertex0.tpaEdges + i0 - tpaEdges])
				continue;
			CGameGraph::CEdge &tEdge0 = tVertex0.tpaEdges[i0];
			for (int i1=0; i1<(int)tVertex1.tNeighbourCount; i1++)
				if ((tEdge0.vertex_id() == tVertex1.tpaEdges[i1].vertex_id()) && !tpAI_Map->q_mark_bit[tVertex1.tpaEdges + i1 - tpaEdges]) {
					tpAI_Map->q_mark_bit[dwaSortOrder[i]] = true;
					for (int j=0; j<(int)tVertex1.tNeighbourCount; j++)
						if (tVertex1.tpaEdges[j].vertex_id() == dwVertex0) {
							tpAI_Map->q_mark_bit[tVertex1.tpaEdges + j - tpaEdges] = true;
							break;
						}
					bOk = false;
					break;
				}
		}
	}
	Progress(1.0f);
}

void vfNormalizeGraph()
{
	for (int i=0, N=(int)tpaGraph.size(); i<N; i++) {
		bool bOk;
		do {
			bOk = true;
			for (int j=1; j<(int)tpaGraph[i].tNeighbourCount; j++)
				if (tpaGraph[i].tpaEdges[j - 1].vertex_id() > tpaGraph[i].tpaEdges[j].vertex_id()) {
					CGameGraph::CEdge tTemp = tpaGraph[i].tpaEdges[j - 1];
					tpaGraph[i].tpaEdges[j - 1] = tpaGraph[i].tpaEdges[j];
					tpaGraph[i].tpaEdges[j] = tTemp;
					bOk = false;
				}
		} 
		while(!bOk);
	}
}

class CGraphSaver {
public:
	CGraphSaver(LPCSTR name, CLevelGraph *tpAI_Map)
	{
		string256					fName;
		strconcat					(fName,name,GAME_LEVEL_GRAPH);
		
		CMemoryWriter				tGraph;
		tGraphHeader.dwVersion		= XRAI_CURRENT_VERSION;
		tGraphHeader.dwLevelCount	= 1;
		tGraphHeader.dwVertexCount	= tpaGraph.size();
		tGraphHeader.dwEdgeCount	= 0;
		//
		{
			for (int i=0; i<(int)tpaGraph.size(); i++)
				tGraphHeader.dwEdgeCount += tpaGraph[i].tNeighbourCount;
		}
		//
		tGraphHeader.dwDeathPointCount = 0;
		tGraphHeader.m_guid			= generate_guid();

		CGameGraph::SLevel			tLevel;
		tLevel.m_offset.set			(0,0,0);
		tLevel.m_id					= 0;
		tLevel.m_name				= name;
		tLevel.m_section			= "";
		tLevel.m_guid				= tpAI_Map->header().guid();
		tGraphHeader.m_levels.insert(std::make_pair(tLevel.m_id,tLevel));
		tGraphHeader.save			(tGraph);

		u32							dwPosition = tGraph.size();
		
		Progress					(0.0f);
		CGameGraph::CVertex			tGraphVertex;
		Memory.mem_fill				(&tGraphVertex,0,sizeof(CGameGraph::CVertex));
		for (int i=0; i<(int)tpaGraph.size(); Progress(float(++i)/tpaGraph.size()/4))
			tGraph.w				(&tGraphVertex,sizeof(CGameGraph::CVertex));
		
		Progress					(0.25f);
		for (int i=0; i<(int)tpaGraph.size(); Progress(.25f + float(++i)/tpaGraph.size()/2)) {
			SDynamicGraphVertex		&tDynamicGraphVertex = tpaGraph[i];
			for (int j=0, k=0; j<(int)tDynamicGraphVertex.tNeighbourCount; j++)
				if (!tpAI_Map->q_mark_bit[tDynamicGraphVertex.tpaEdges + j - tpaEdges]) {
					k++;
					tGraph.w_u32	(tDynamicGraphVertex.tpaEdges[j].vertex_id());	
					tGraph.w_float	(tDynamicGraphVertex.tpaEdges[j].distance());	
				}
			tDynamicGraphVertex.tNeighbourCount = k;
		}
		
		Progress					(.75f);
		tGraph.seek					(dwPosition);
		for (int i=0, j=0, k=tpaGraph.size()*sizeof(CGameGraph::CVertex); i<(int)tpaGraph.size(); j += tpaGraph[i].tNeighbourCount, Progress(.75f + float(++i)/tpaGraph.size()/4)) {
			SDynamicGraphVertex				&tDynamicGraphVertex = tpaGraph[i];
			tGraphVertex.tLocalPoint		= tDynamicGraphVertex.tLocalPoint;
			tGraphVertex.tGlobalPoint		= tDynamicGraphVertex.tGlobalPoint;
			tGraphVertex.tNodeID			= tDynamicGraphVertex.tNodeID;
			Memory.mem_copy					(tGraphVertex.tVertexTypes,tDynamicGraphVertex.tVertexTypes,GameGraph::LOCATION_TYPE_COUNT*sizeof(GameGraph::_LOCATION_ID));
			tGraphVertex.tLevelID			= tDynamicGraphVertex.tLevelID;
			tGraphVertex.tNeighbourCount	= tDynamicGraphVertex.tNeighbourCount;
			tGraphVertex.dwEdgeOffset		= k + j*(u32)sizeof(CGameGraph::CEdge);
			tGraphVertex.tDeathPointCount	= tDynamicGraphVertex.tDeathPointCount;
			tGraphVertex.dwPointOffset		= tDynamicGraphVertex.dwPointOffset;
			tGraph.w						(&tGraphVertex,	sizeof(CGameGraph::CVertex));
		}
		
		u32							dwNewEdgeCount = 0;
		for (int i=0; i<(int)tpaGraph.size(); i++)
			dwNewEdgeCount += tpaGraph[i].tNeighbourCount;
		Msg							("%d edges are removed",tGraphHeader.dwEdgeCount - dwNewEdgeCount);
		Msg							("%d vertices in graph",tGraphHeader.dwVertexCount);
		Msg							("%d edges in graph",dwNewEdgeCount);

		tGraph.seek					(12);
		tGraph.w_u32				(dwNewEdgeCount);

		tGraph.save_to				(fName);
		Progress					(1.0f);
		Msg							("%d bytes saved",int(tGraph.size()));
	}
};

void vfRemoveIncoherentGraphPoints(CLevelGraph *tpAI_Map, u32 &dwVertexCount)
{
	xr_vector<bool>		tMarks;
	xr_vector<bool>		tDisconnected;
	tDisconnected.assign(tpaGraph.size(),false);
	for (int k=0, j=0, n=(int)tpaGraph.size(); k<n; k++) {
		tMarks.assign	(tpAI_Map->header().vertex_count(),false);
		vfRecurseMark	(*tpAI_Map,tMarks,tpaGraph[k].tNodeID);
		for (int i=0; i<n; i++)
			if (!tMarks[tpaGraph[i].tNodeID]) {
				tDisconnected[k] = true;
				Msg		("Graph point %d [%f][%f][%f] is disconnected",k,VPUSH(tpaGraph[k].tLocalPoint));
				j++;
				break;
			}
	}
	
	for (int i=0, k=0; i<n; i++, k++)
		if (tDisconnected[k]) {
			tpaGraph.erase(tpaGraph.begin() + i);
			i--;
			n--;
			dwVertexCount--;
		}
	Msg					("%d graph points are incoherent (they are removed)",j);
	Msg					("%d graph points are valid",dwVertexCount);
}

//extern void	xrBuildCrossTable	(LPCSTR name);
//
//void vfSaveEmptyGraph			(LPCSTR name)
//{
//	string256					fName;
//	strconcat					(fName,name,GAME_LEVEL_GRAPH);
//
//	CMemoryWriter				tGraph;
//	tGraphHeader.dwVersion		= XRAI_CURRENT_VERSION;
//	tGraphHeader.dwLevelCount	= 1;
//	tGraphHeader.dwVertexCount	= tpaGraph.size();
//	tGraphHeader.dwEdgeCount	= 0;
//	tGraphHeader.dwDeathPointCount = 0;
//
//	CGameGraph::SLevel			tLevel;
//	tLevel.tOffset.set			(0,0,0);
//	tLevel.tLevelID				= 0;
//	Memory.mem_copy				(tLevel.caLevelName,name,(u32)xr_strlen(name) + 1);
//	tGraphHeader.tpLevels.insert(std::make_pair(tLevel.tLevelID,tLevel));
//	tGraph.w_u32				(tGraphHeader.dwVersion);
//	tGraph.w_u32				(tGraphHeader.dwLevelCount);
//	tGraph.w_u32				(tGraphHeader.dwVertexCount);
//	tGraph.w_u32				(tGraphHeader.dwEdgeCount);
//	tGraph.w_u32				(tGraphHeader.dwDeathPointCount);
//	GameGraph::LEVEL_MAP::iterator	I = tGraphHeader.tpLevels.begin();
//	GameGraph::LEVEL_MAP::iterator	E = tGraphHeader.tpLevels.end();
//	for ( ; I != E; I++) {
//		tGraph.w_stringZ		((*I).second.caLevelName);
//		tGraph.w_fvector3		((*I).second.tOffset);
//		tGraph.w_u32			((*I).second.tLevelID);
//	}
//
//	CGameGraph::CVertex			tGraphVertex;
//	for (int i=0, j=0, k=tpaGraph.size()*sizeof(CGameGraph::CVertex); i<(int)tpaGraph.size(); j += tpaGraph[i].tNeighbourCount, Progress(.75f + float(++i)/tpaGraph.size()/4)) {
//		SDynamicGraphVertex				&tDynamicGraphVertex = tpaGraph[i];
//		tGraphVertex.tLocalPoint		= tDynamicGraphVertex.tLocalPoint;
//		tGraphVertex.tGlobalPoint		= tDynamicGraphVertex.tGlobalPoint;
//		tGraphVertex.tNodeID			= tDynamicGraphVertex.tNodeID;
//		Memory.mem_copy					(tGraphVertex.tVertexTypes,tDynamicGraphVertex.tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
//		tGraphVertex.tLevelID			= tDynamicGraphVertex.tLevelID;
//		tGraphVertex.tNeighbourCount	= tDynamicGraphVertex.tNeighbourCount;
//		tGraphVertex.dwEdgeOffset		= k + j*(u32)sizeof(CGameGraph::CEdge);
//		tGraphVertex.tDeathPointCount	= tDynamicGraphVertex.tDeathPointCount;
//		tGraphVertex.dwPointOffset		= tDynamicGraphVertex.dwPointOffset;
//		tGraph.w						(&tGraphVertex,	sizeof(CGameGraph::CVertex));
//	}
//
//	tGraph.save_to				(fName);
//}
//
struct sort_by_node_id_predicate {
	IC	bool	operator()	(const SDynamicGraphVertex &v0, const SDynamicGraphVertex &v1) const
	{
		return				(v0.tNodeID < v1.tNodeID);
	}
};

struct compare_by_node_id_predicate {
	IC	bool	operator()	(const SDynamicGraphVertex &v0, const SDynamicGraphVertex &v1) const
	{
		return				(v0.tNodeID == v1.tNodeID);
	}
};

void xrBuildGraph(LPCSTR name)
{
	CThreadManager			tThreadManager;		// multithreading
	xrCriticalSection		tCriticalSection;	// thread synchronization
	CLevelGraph				*tpAI_Map;

	Msg("Building Level %s",name);

	Phase("Loading AI map");
	Progress(0.0f);
	tpAI_Map = xr_new<CLevelGraph>(name);
	Progress(1.0f);
	Msg("%d nodes loaded",int(tpAI_Map->header().vertex_count()));
	
	tpaGraph.clear();

	Phase("Loading graph points");
	vfLoadGraphPoints(name);
	u32 dwGraphPoints;
	Msg("%d vertices are loaded",int(dwGraphPoints = tpaGraph.size()));

	Phase("Removing duplicate graph points");
	vfRemoveDuplicateGraphPoints(dwGraphPoints);

	Phase("Searching AI map for corresponding nodes");
	xr_vector<SDynamicGraphVertex>::iterator	I = tpaGraph.begin();
	xr_vector<SDynamicGraphVertex>::iterator	E = tpaGraph.end();
	for ( ; I != E; ++I)
		(*I).tNodeID	= tpAI_Map->vertex_id((*I).tLocalPoint);
	Progress(1.0f);
	Msg("%d points don't have corresponding nodes (they are removed)",dwfErasePoints());
	FlushLog();

	std::sort			(tpaGraph.begin(),tpaGraph.end(),sort_by_node_id_predicate());
	I					= std::unique(tpaGraph.begin(),tpaGraph.end(),compare_by_node_id_predicate());
	Msg					("%d points have the same corresponding nodes (they are removed)",tpaGraph.size() - (I - tpaGraph.begin()));
	tpaGraph.erase		(I,tpaGraph.end());

	dwGraphPoints = tpaGraph.size();
//	we shouldn't do this, since AI-map should be verified
//	Phase("Removing incoherent graph points");
//	vfRemoveIncoherentGraphPoints(tpAI_Map,dwGraphPoints);

	R_ASSERT2				(tpaGraph.size(), "There are no graph points!");

#pragma todo("Dima to Dima : make removing incoherent points faster")
#pragma todo("Dima to Dima : make graph building faster")
	Phase("Allocating memory");
	vfAllocateMemory();

	Phase("Building graph");
	for (u32 thID=0, dwThreadCount = THREAD_COUNT, N = tpaGraph.size(), M = 0, K = 0; thID<dwThreadCount; M += K, thID++)
		tThreadManager.start(xr_new<CGraphThread>(thID,M, ((thID + 1) == dwThreadCount) ? N - 1 : M + (K = GET_INDEX((N - M),(dwThreadCount - thID))),MAX_DISTANCE_TO_CONNECT,&tCriticalSection,tpAI_Map));
//		xr_new<CGraphThread>(thID,M, ((thID + 1) == dwThreadCount) ? N - 1 : M + (K = GET_INDEX((N - M),(dwThreadCount - thID))),MAX_DISTANCE_TO_CONNECT,&tCriticalSection,tpAI_Map)->Execute();
	tThreadManager.wait();
	
	for (int i=0, dwEdgeCount=0; i<(int)tpaGraph.size(); i++)
		dwEdgeCount += tpaGraph[i].tNeighbourCount;
	Msg("%d edges built",dwEdgeCount);
	Progress(1.0f);

	Phase("Normalizing graph");
	vfNormalizeGraph();

	Phase("Checking graph connectivity");
	if ((dwEdgeCount < ((int)tpaGraph.size() - 1)) || !bfCheckForGraphConnectivity(tpAI_Map)) {
		Msg("Graph is not connected!");
		extern  HWND logWindow;
		MessageBox	(logWindow,"Graph is not connected","Data mismatch",MB_OK|MB_ICONWARNING);
		return;
	}
	else
		Msg("Graph is connected");
	
	Phase("Preprocessing edges");
	vfPreprocessEdges(dwEdgeCount);

	Phase("Sorting edges");
	Progress(0.0f);
	vfQuickSortEdges(tpaEdges,dwaSortOrder,dwEdgeCount);
	Progress(1.0f);

	Phase("Optimizing graph");
	vfOptimizeGraph(dwEdgeCount,tpAI_Map);

	Phase("Saving graph");
	::CGraphSaver	tGraphSaver(name,tpAI_Map);

	Phase("Freeing graph being built");
	
	Progress		(0.0f);
	xr_free			(tpaEdges);
	xr_free			(dwaSortOrder);
	xr_delete		(tpAI_Map);
	tpaGraph.clear	();
	
	Progress		(1.0f);
	
	Msg("\nBuilding level %s successfully completed",name);
}
