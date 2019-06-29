////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph_debug.cpp
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef DEBUG
#ifndef AI_COMPILER

#include "level_navigation_graph.h"
#include "level_navigation_graph_space.h"
#include "graph_abstract.h"
#include "ai_debug.h"

#define sector_min(sector)		vertex_position((sector).min_vertex_id())
#define sector_max(sector)		vertex_position((sector).max_vertex_id())
#define sector_center(sector)	Fvector().average(sector_min(sector),sector_max(sector))

void CLevelNavigationGraph::render	()
{
	inherited::render	();

	if (!psAI_Flags.test(aiSectors))
		return;

	CSectorGraph::const_vertex_iterator	I = sectors().vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sectors().vertices().end();
	for ( ; I != E; ++I) {
		const CSectorGraph::CVertex	&vertex = *(*I).second;
		const CSector				&sector = vertex.data();

		Fvector						min		= sector_min(sector);
		Fvector						center	= sector_center(sector);
		
		RCache.dbg_DrawAABB	(
			center,
			_max(center.x - min.x - .05f,.05f),
			_max(center.y - min.y - .05f,.05f),
			_max(center.z - min.z - .05f,.05f),
			D3DCOLOR_XRGB(0,255,0)
		);

		CSectorGraph::const_iterator	i = vertex.edges().begin();
		CSectorGraph::const_iterator	e = vertex.edges().end();
		for ( ; i != e; ++i) {
			RCache.dbg_DrawLINE	(
				Fidentity,
				center,
				sector_center((*i).vertex()->data()),
				D3DCOLOR_XRGB(0,255,255)
			);
		}
	}
}

#endif
#endif