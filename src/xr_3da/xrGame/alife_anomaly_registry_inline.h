////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomaly_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife anomaly registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "game_graph.h"

IC	void CALifeAnomalyRegistry::add					(CALifeKnownAnomaly *anomaly)
{
	m_anomalies[anomaly->game_vertex_id()].push_back	(anomaly);
	m_anomaly_cross_table[anomaly->type()].push_back	(anomaly);
}

IC	const ALife::ANOMALY_P_VECTOR_SVECTOR &CALifeAnomalyRegistry::cross	() const
{
	return							(m_anomaly_cross_table);
}
