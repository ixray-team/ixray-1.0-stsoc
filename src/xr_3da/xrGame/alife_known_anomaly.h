////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_known_anomaly.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife known anomaly class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"
#include "game_graph_space.h"

class CALifeKnownAnomaly : public IPureSerializeObject<IReader,IWriter> {
protected:
	ALife::EAnomalousZoneType			m_type;
	float								m_power;
	float								m_distance;
	GameGraph::_GRAPH_ID					m_game_vertex_id;

public:
	IC									CALifeKnownAnomaly	();
	virtual								~CALifeKnownAnomaly	();
	virtual void						save				(IWriter	&memory_stream);
	virtual void						load				(IReader	&file_stream);
	IC		ALife::EAnomalousZoneType	type				() const;
	IC		float						power				() const;
	IC		float						distance			() const;
	IC		GameGraph::_GRAPH_ID			game_vertex_id		() const;
	IC		void						set_type			(const ALife::EAnomalousZoneType &type);
	IC		void						set_power			(float power);
	IC		void						set_distance		(float distance);
	IC		void						set_game_vertex_id	(const GameGraph::_GRAPH_ID &game_vertex_id);
};

#include "alife_known_anomaly_inline.h"