////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_criteria.h
//	Created 	: 04.12.2003
//  Modified 	: 04.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager criteria path builder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "ai_space.h"

void CDetailPathManager::build_criteria_path	(const xr_vector<u32> &level_path, u32 intermediate_index)
{
	VERIFY					(!level_path.empty());
	STravelPathPoint		current,next;

	// start point
	m_path.clear			();
	current.velocity		= 0;
	current.position		= m_start_position;
	m_path.push_back		(current);

	// end point
	Fvector					Last = m_dest_position;

	// segmentation
	CLevelGraph::SContour	Ccur,Cnext;
	ai().level_graph().contour(Ccur,level_path[0]);
	m_segments.clear		();
	for (u32 I=1; I<level_path.size(); ++I) {
		CLevelGraph::SSegment		S;
		ai().level_graph().contour	(Cnext,level_path[I]);
		ai().level_graph().intersect(S,Ccur,Cnext);
		m_segments.push_back(S);
		Ccur				= Cnext;
	}

	// path building
	for (I=0; I<m_segments.size(); ++I)
	{
		// build probe point
		CLevelGraph::SSegment		&S = m_segments[I];
		ai().level_graph().nearest(next.position,current.position,S.v1,S.v2);

		// select far point
		Fvector				fp	= Last;
		if ((I+1)<m_segments.size())	{
			CLevelGraph::SSegment	&F = m_segments[I+1];
			ai().level_graph().nearest(fp,current.position,F.v1,F.v2);
		}

		// try to cast a line from 'current.position' to 'fp'
		Fvector	ip;
		if (ai().level_graph().intersect(ip,S.v1,S.v2,current.position,fp)) {
			next.position.set(ip);
		} else {
			// find nearest point to segment 'current.position' to 'fp'
			float			d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.position);
			float			d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.position);
			if (d1<d2)		next.position.set(S.v1);
			else			next.position.set(S.v2);
		}

		// record _new point
		if (!next.position.similar(m_path.back().position))	m_path.push_back(next);
		current				= next;
	}
	next.position.set		(Last);
	if (!next.position.similar(m_path.back().position))	m_path.push_back(next);

	// setup variables
	m_current_travel_point	= 0;
}
