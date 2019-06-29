////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomaly_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife anomaly registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_anomaly_registry.h"
#include "object_broker.h"
#include "ai_space.h"
#include "game_graph.h"

CALifeAnomalyRegistry::CALifeAnomalyRegistry	(LPCSTR section)
{
	m_anomalies.resize				(ai().game_graph().header().vertex_count());
	m_anomaly_cross_table.resize	(ALife::eAnomalousZoneTypeDummy);
}

void CALifeAnomalyRegistry::clear				()
{
	delete_data						(m_anomalies);
	delete_data						(m_anomaly_cross_table);
	m_anomalies.resize				(ai().game_graph().header().vertex_count());
	m_anomaly_cross_table.resize	(ALife::eAnomalousZoneTypeDummy);
}

CALifeAnomalyRegistry::~CALifeAnomalyRegistry	()
{
	delete_data					(m_anomalies);
}

void CALifeAnomalyRegistry::save				(IWriter &memory_stream)
{
	Msg							("* Saving anomalies...");
	memory_stream.open_chunk	(ANOMALY_CHUNK_DATA);
	
	u32							count = 0;
	ALife::ANOMALY_P_VECTOR_IT	I = m_anomalies.begin(), B = I;
	ALife::ANOMALY_P_VECTOR_IT	E = m_anomalies.end();
	for ( ; I != E; ++I)
		if ((*I).size())
			++count;

	memory_stream.w_u32			(count);
	for (I = B; I != E; ++I)
		if ((*I).size()) {
			memory_stream.w_u32	(u32(I - B));
			save_data			(*I,memory_stream);
		}

	memory_stream.close_chunk	();
}

void CALifeAnomalyRegistry::load				(IReader &file_stream)
{ 
	Msg							("* Loading anomalies...");
	R_ASSERT2					(file_stream.find_chunk(ANOMALY_CHUNK_DATA),"Can't find chunk ANOMALY_CHUNK_DATA!");

	u32							count = file_stream.r_u32();
	for (u32 i=0; i<count; ++i) {
		u32						index = file_stream.r_u32();
		load_data				(m_anomalies[index],file_stream);
	}
}
