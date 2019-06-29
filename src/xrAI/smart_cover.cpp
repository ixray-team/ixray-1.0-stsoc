////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 20.03.2004
//  Modified 	: 20.03.2004
//	Author		: Dmitriy Iassenev
//	Description : precompiled header
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "quadtree.h"
#include "level_graph.h"
#include "object_broker.h"

class CCoverPoint {
public:
	Fvector			m_position;
	u32				m_level_vertex_id;
	u8				m_cover[4][2];

	IC					CCoverPoint	(const Fvector &point, u32 level_vertex_id) :
							m_position			(point),
							m_level_vertex_id	(level_vertex_id)
	{
	}

	IC	const Fvector	&position	() const
	{
		return		(m_position);
	}

	IC	u32				level_vertex_id	() const
	{
		return		(m_level_vertex_id);
	}

	IC	bool			operator==	(const CCoverPoint &point) const
	{
		return		(!!position().similar(point.position()));
	}
};

typedef CQuadTree<CCoverPoint>				CCoverQuadTree;

bool edge_vertex(CLevelGraph *graph, u32 index)
{
	CLevelGraph::CVertex	*v = graph->vertex(index);
	return					(
		!graph->valid_vertex_id(v->link(0)) ||
		!graph->valid_vertex_id(v->link(1)) ||
		!graph->valid_vertex_id(v->link(2)) ||
		!graph->valid_vertex_id(v->link(3))
	);
}

bool cover(CLevelGraph *graph, CLevelGraph::CVertex *v, u32 index0, u32 index1, xr_vector<bool> &covers)
{
	return					(
		graph->valid_vertex_id(v->link(index0)) &&
		graph->valid_vertex_id(graph->vertex(v->link(index0))->link(index1)) &&
		covers[graph->vertex(v->link(index0))->link(index1)]
	);
}

bool critical_point(CLevelGraph *graph, CLevelGraph::CVertex *v, u32 index, u32 index0, u32 index1, xr_vector<bool> &covers)
{
	return					(
		!graph->valid_vertex_id(v->link(index)) &&
		(
			!graph->valid_vertex_id(v->link(index0)) || 
			!graph->valid_vertex_id(v->link(index1)) ||
			cover(graph,v,index0,index,covers) || 
			cover(graph,v,index1,index,covers)
		)
	);
}

bool critical_cover(CLevelGraph *graph, u32 index, xr_vector<bool> &covers)
{
	CLevelGraph::CVertex	*v = graph->vertex(index);
	return					(
		critical_point(graph,v,0,1,3,covers) || 
		critical_point(graph,v,2,1,3,covers) || 
		critical_point(graph,v,1,0,2,covers) || 
		critical_point(graph,v,3,0,2,covers)
	);
}

char show(CLevelGraph *graph, u32 index, int i, int j, xr_vector<bool> &covers)
{
	if (!i && !j)
		return				('0');

	if (!i)
		if (j == -1)
			return			(!graph->valid_vertex_id(graph->vertex(index)->link(0)) ? '*' : covers[graph->vertex(index)->link(0)] ? '0' : '1');
		else
			return			(!graph->valid_vertex_id(graph->vertex(index)->link(2)) ? '*' : covers[graph->vertex(index)->link(2)] ? '0' : '1');

	if (!j)
		if (i == -1)
			return			(!graph->valid_vertex_id(graph->vertex(index)->link(3)) ? '*' : covers[graph->vertex(index)->link(3)] ? '0' : '1');
		else
			return			(!graph->valid_vertex_id(graph->vertex(index)->link(1)) ? '*' : covers[graph->vertex(index)->link(1)] ? '0' : '1');

	if (i == -1)
		if (j == -1) {
			if (graph->valid_vertex_id(graph->vertex(index)->link(3)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(3))->link(0)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(3))->link(0)] ? '0' : '1');

			if (graph->valid_vertex_id(graph->vertex(index)->link(0)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(0))->link(3)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(0))->link(3)] ? '0' : '1');
		}
		else {
			if (graph->valid_vertex_id(graph->vertex(index)->link(3)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(3))->link(2)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(3))->link(2)] ? '0' : '1');

			if (graph->valid_vertex_id(graph->vertex(index)->link(2)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(2))->link(3)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(2))->link(3)] ? '0' : '1');
		}
	else
		if (j == -1) {
			if (graph->valid_vertex_id(graph->vertex(index)->link(1)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(1))->link(0)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(1))->link(0)] ? '0' : '1');

			if (graph->valid_vertex_id(graph->vertex(index)->link(0)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(0))->link(1)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(0))->link(1)] ? '0' : '1');
		}
		else {
			if (graph->valid_vertex_id(graph->vertex(index)->link(1)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(1))->link(2)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(1))->link(2)] ? '0' : '1');

			if (graph->valid_vertex_id(graph->vertex(index)->link(2)))
				if (!graph->valid_vertex_id(graph->vertex(graph->vertex(index)->link(2))->link(1)))
					return	('*');
				else
					return	(covers[graph->vertex(graph->vertex(index)->link(2))->link(1)] ? '0' : '1');
		}

	return					('?');
}

void show(CLevelGraph *graph, const CCoverPoint *point, xr_vector<bool> &covers)
{
	Msg						("[%f][%f][%f]",VPUSH(point->position()));
	Msg						("%2c%2c%2c",show(graph,point->level_vertex_id(),-1,-1,covers),show(graph,point->level_vertex_id(),0,-1,covers),show(graph,point->level_vertex_id(),1,-1,covers));
	Msg						("%2c%2c%2c",show(graph,point->level_vertex_id(),-1, 0,covers),show(graph,point->level_vertex_id(),0, 0,covers),show(graph,point->level_vertex_id(),1, 0,covers));
	Msg						("%2c%2c%2c",show(graph,point->level_vertex_id(),-1, 1,covers),show(graph,point->level_vertex_id(),0, 1,covers),show(graph,point->level_vertex_id(),1, 1,covers));
}

void smart_cover(LPCSTR project_name)
{
	CLevelGraph				*graph	= xr_new<CLevelGraph>(project_name);
	CCoverQuadTree			*tree	= xr_new<CCoverQuadTree>(graph->header().box(),graph->header().cell_size()*.5f,65536,65536);

//	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);

//	u64						s,f;
//	s						= CPU::GetCycleCount();

	u32						n = graph->header().vertex_count();
	xr_vector<bool>			covers;
	covers.resize			(n);

	for (u32 i=0; i<n; ++i)
		covers[i]			= edge_vertex(graph,i);
	
	for (u32 i=0; i<n; ++i)
		if (covers[i] && critical_cover(graph,i,covers))
			tree->insert	(xr_new<CCoverPoint>(graph->vertex_position(graph->vertex(i)),i));

//	f						= CPU::GetCycleCount();
//	Msg						("Total build time %f (%d tests : %f)",CPU::cycles2seconds*float(f - s),n,CPU::cycles2microsec*float(f - s)/float(n));
//	Msg						("Total cover points %d",tree->size());

	xr_vector<CCoverPoint*>	nearest;
//	nearest.reserve			(tree->size());
//	tree->nearest			(graph->vertex_position(graph->vertex(u32(0))),10000.f,nearest);
//	xr_vector<CCoverPoint*>::iterator I = nearest.begin();
//	xr_vector<CCoverPoint*>::iterator E = nearest.end();
//	for ( ; I != E; ++I) {
//		show				(graph,*I,covers);
//		xr_delete			(*I);
//	}

//	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
//	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	tree->all				(nearest);
	delete_data				(nearest);
	xr_delete				(tree);
	xr_delete				(graph);
}