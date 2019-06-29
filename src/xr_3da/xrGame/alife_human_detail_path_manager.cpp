////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_detail_path_manager.cpp
//	Created 	: 01.11.2005
//  Modified 	: 01.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human detail path manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_detail_path_manager.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "game_graph.h"
#include "level_graph.h"
#include "game_level_cross_table.h"
#include "alife_smart_terrain_task.h"
#include "alife_graph_registry.h"
#include "graph_engine.h"
#include "alife_human_brain.h"

CALifeHumanDetailPathManager::CALifeHumanDetailPathManager	(object_type *object)
{
	VERIFY							(object);
	m_object						= object;
	m_last_update_time				= 0;
	m_destination.m_game_vertex_id	= this->object().m_tGraphID;
	m_destination.m_level_vertex_id	= this->object().m_tNodeID;
	m_destination.m_position		= this->object().o_Position;
	m_walked_distance				= 0.f;
	speed							(this->object().m_fGoingSpeed);
}

void CALifeHumanDetailPathManager::target					(const GameGraph::_GRAPH_ID &game_vertex_id, const u32 &level_vertex_id, const Fvector &position)
{
	VERIFY							(ai().game_graph().valid_vertex_id(game_vertex_id));
	VERIFY							((ai().game_graph().vertex(m_destination.m_game_vertex_id)->level_id() != ai().alife().graph().level().level_id()) || ai().level_graph().valid_vertex_id(level_vertex_id));
	VERIFY							((ai().game_graph().vertex(m_destination.m_game_vertex_id)->level_id() != ai().alife().graph().level().level_id()) || (ai().cross_table().vertex(level_vertex_id).game_vertex_id() == game_vertex_id));
	VERIFY							((ai().game_graph().vertex(m_destination.m_game_vertex_id)->level_id() != ai().alife().graph().level().level_id()) || ai().level_graph().inside(level_vertex_id,position));

	m_destination.m_game_vertex_id	= game_vertex_id;
	m_destination.m_level_vertex_id	= level_vertex_id;
	m_destination.m_position		= position;
}

void CALifeHumanDetailPathManager::target					(const GameGraph::_GRAPH_ID &game_vertex_id)
{
	VERIFY							(ai().game_graph().valid_vertex_id(game_vertex_id));
	target							(game_vertex_id,ai().game_graph().vertex(game_vertex_id)->level_vertex_id(),ai().game_graph().vertex(game_vertex_id)->level_point());
}

void CALifeHumanDetailPathManager::target					(const CALifeSmartTerrainTask &task)
{
	target							(task.game_vertex_id(),task.level_vertex_id(),task.position());
}

void CALifeHumanDetailPathManager::target					(const CALifeSmartTerrainTask *task)
{
	target							(*task);
}

bool CALifeHumanDetailPathManager::completed				() const
{
	if (m_destination.m_game_vertex_id != object().m_tGraphID)
		return						(false);

	if (m_destination.m_level_vertex_id != object().m_tNodeID)
		return						(false);

	return							(true);
}

bool CALifeHumanDetailPathManager::actual					() const
{
	if (failed())
		return						(false);

	if (m_destination.m_game_vertex_id != m_path.front())
		return						(false);

	return							(true);
}

bool CALifeHumanDetailPathManager::failed					() const
{
	return							(m_path.empty());
}

void CALifeHumanDetailPathManager::update					()
{
	ALife::_TIME_ID					current_time = ai().alife().time_manager().game_time();
	VERIFY							(current_time >= m_last_update_time);
	if (current_time == m_last_update_time)
		return;

	ALife::_TIME_ID					time_delta = current_time - m_last_update_time;
	update							(time_delta);
	// we advisedly "lost" time we need to process a query to avoid some undesirable effects
	m_last_update_time				= ai().alife().time_manager().game_time();
}

void CALifeHumanDetailPathManager::actualize				()
{
	m_path.clear					();

	bool							failed = !ai().graph_engine().search	(
		ai().game_graph(),
		object().m_tGraphID,
		m_destination.m_game_vertex_id,
		&m_path,
		GraphEngineSpace::CBaseParameters()
	);

	if (failed)
		return;

	if (m_path.size() == 1)
		return;

	m_walked_distance				= 0.f;
	std::reverse					(m_path.begin(),m_path.end());
}

void CALifeHumanDetailPathManager::update					(const ALife::_TIME_ID &time_delta)
{
	// first update has enormous time delta, therefore just skip it
	if (!m_last_update_time)
		return;

	if (completed())
		return;

	if (!actual()) {
		actualize					();

		if (failed())
			return;
	}

	follow_path						(time_delta);
}

void CALifeHumanDetailPathManager::follow_path				(const ALife::_TIME_ID &time_delta)
{
	VERIFY							(!completed());
	VERIFY							(!failed());
	VERIFY							(actual());
	VERIFY							(!m_path.empty());
	VERIFY							(m_path.back() == object().m_tGraphID);

	if (m_path.size() == 1) {
		VERIFY						(object().m_tGraphID == m_destination.m_game_vertex_id);
		m_walked_distance			= 0.f;
		object().m_tNodeID			= m_destination.m_level_vertex_id;
		object().o_Position			= m_destination.m_position;
#ifdef DEBUG
		object().m_fDistanceFromPoint	= 0.f;
		object().m_fDistanceToPoint		= 0.f;
		object().m_tNextGraphID			= object().m_tGraphID;
#endif
		return;
	}

	float							update_distance = ((float(time_delta)/1000.f)/ai().alife().time_manager().normal_time_factor())*speed();
	for ( ; m_path.size() > 1;) {
		float						distance_between = ai().game_graph().distance(object().m_tGraphID,(GameGraph::_GRAPH_ID)m_path[m_path.size() - 2]);
		if (distance_between > (update_distance + m_walked_distance)) {
			m_walked_distance		+= update_distance;
#ifdef DEBUG
			object().m_fDistanceFromPoint	= m_walked_distance;
			object().m_fDistanceToPoint		= distance_between;
			object().m_tNextGraphID			= (GameGraph::_GRAPH_ID)m_path[m_path.size() - 2];
#endif
			return;
		}

		update_distance				+= m_walked_distance;
		update_distance				-= distance_between;
		m_walked_distance			= 0.f;
		m_path.pop_back				();
		object().alife().graph().change		(&object(),object().m_tGraphID,(GameGraph::_GRAPH_ID)m_path.back());
		object().brain().on_location_change	();
	}
}
