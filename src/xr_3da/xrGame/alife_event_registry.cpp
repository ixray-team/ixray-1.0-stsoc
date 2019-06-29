////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_event_registry.h"

CALifeEventRegistry::~CALifeEventRegistry	()
{
	delete_data					(m_events);
}

void CALifeEventRegistry::save				(IWriter &memory_stream)
{
	Msg							("* Saving events...");
	memory_stream.open_chunk	(EVENT_CHUNK_DATA);
	memory_stream.w				(&m_id,sizeof(m_id));
	save_data					(m_events,memory_stream);
	memory_stream.close_chunk	();
}
