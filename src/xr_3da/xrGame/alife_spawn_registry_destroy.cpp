////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_destroy.cpp
//	Created 	: 19.10.2004
//  Modified 	: 19.10.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry destroy routines
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_spawn_registry.h"

IC	bool CALifeSpawnRegistry::redundant			(CSE_Abstract &abstract)
{
	return									(!!abstract.m_spawn_flags.test(CSE_Abstract::flSpawnDestroyOnSpawn));
}

void CALifeSpawnRegistry::fill_redundant_spawns	(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &spawns, bool parent_redundant)
{
	VERIFY									(vertex);

	bool									redundance = parent_redundant || redundant(vertex->data()->object());
	if (vertex->edges().empty()) {
		if (redundance)
			spawns.push_back				(vertex->data()->object().m_tSpawnID);
		return;
	}

	SPAWN_GRAPH::const_iterator				I = vertex->edges().begin();
	SPAWN_GRAPH::const_iterator				E = vertex->edges().end();
	for ( ; I != E; ++I)
		fill_redundant_spawns				(m_spawns.vertex((*I).vertex_id()),spawns,redundance);
}

void CALifeSpawnRegistry::fill_redundant_spawns	(xr_vector<ALife::_SPAWN_ID> &spawns)
{
	xr_vector<ALife::_SPAWN_ID>::iterator	I = m_spawn_roots.begin();
	xr_vector<ALife::_SPAWN_ID>::iterator	E = m_spawn_roots.end();
	for ( ; I != E; ++I)
		fill_redundant_spawns				(m_spawns.vertex(*I),spawns);

	process_spawns							(spawns);
}
