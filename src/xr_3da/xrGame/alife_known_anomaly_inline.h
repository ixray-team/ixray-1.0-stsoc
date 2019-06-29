////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_known_anomaly_inline.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife known anomaly class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeKnownAnomaly::CALifeKnownAnomaly								()
{
}

IC	ALife::EAnomalousZoneType	CALifeKnownAnomaly::type				() const
{
	return						(m_type);
}

IC	float						CALifeKnownAnomaly::power				() const
{
	return						(m_power);
}

IC	float						CALifeKnownAnomaly::distance			() const
{
	return						(m_distance);
}

IC	GameGraph::_GRAPH_ID			CALifeKnownAnomaly::game_vertex_id		() const
{
	return						(m_game_vertex_id);
}

IC	void						CALifeKnownAnomaly::set_type			(const ALife::EAnomalousZoneType &type)
{
	m_type						= type;
}

IC	void						CALifeKnownAnomaly::set_power			(float power)
{
	m_power						= power;
}

IC	void						CALifeKnownAnomaly::set_distance		(float distance)
{
	m_distance					= distance;
}

IC	void						CALifeKnownAnomaly::set_game_vertex_id	(const GameGraph::_GRAPH_ID &game_vertex_id)
{
	m_game_vertex_id			= game_vertex_id;
}
