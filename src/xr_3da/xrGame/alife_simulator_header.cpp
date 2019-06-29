////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_header.cpp
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator header
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator_header.h"

CALifeSimulatorHeader::~CALifeSimulatorHeader	()
{
}

void CALifeSimulatorHeader::save				(IWriter	&memory_stream)
{
	memory_stream.open_chunk	(ALIFE_CHUNK_DATA);
	memory_stream.w				(&m_version,	sizeof(m_version));
	memory_stream.close_chunk	();
}

void CALifeSimulatorHeader::load				(IReader	&file_stream)
{
	R_ASSERT2					(file_stream.find_chunk(ALIFE_CHUNK_DATA),"Can't find chunk ALIFE_CHUNK_DATA");
	file_stream.r				(&m_version,	sizeof(m_version));
	R_ASSERT2					(ALIFE_VERSION == m_version,"ALife version mismatch! (Delete saved game and try again)");
};
