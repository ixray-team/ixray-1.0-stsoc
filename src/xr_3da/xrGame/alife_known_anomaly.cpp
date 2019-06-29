////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_known_anomaly.cpp
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife known anomaly class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_known_anomaly.h"

CALifeKnownAnomaly::~CALifeKnownAnomaly	()
{
}

void CALifeKnownAnomaly::save	(IWriter &memory_stream)
{
	memory_stream.w				(&m_type,			sizeof(m_type));
	memory_stream.w_float		(m_power);
	memory_stream.w_float		(m_distance);
	memory_stream.w				(&m_game_vertex_id,	sizeof(m_game_vertex_id));
}

void CALifeKnownAnomaly::load	(IReader &file_stream)
{
	file_stream.r				(&m_type,			sizeof(m_type));
	m_power						= file_stream.r_float();
	m_distance					= file_stream.r_float();
	file_stream.r				(&m_game_vertex_id,	sizeof(m_game_vertex_id));
};
