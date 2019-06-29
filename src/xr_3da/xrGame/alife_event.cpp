////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event.cpp
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_event.h"
#include "alife_event_group.h"

CALifeEvent::~CALifeEvent	()
{
}

void CALifeEvent::load		(IReader &stream)
{
	stream.r				(&m_tEventID,		sizeof(m_tEventID));
	stream.r				(&m_tTimeID,		sizeof(m_tTimeID));
	stream.r				(&m_tGraphID,		sizeof(m_tGraphID));
	stream.r				(&m_tCombatResult,	sizeof(m_tCombatResult));
	m_tpMonsterGroup1->load	(stream);
	m_tpMonsterGroup2->load	(stream);
}

void CALifeEvent::save		(IWriter &stream)
{
	stream.w				(&m_tEventID,		sizeof(m_tEventID));
	stream.w				(&m_tTimeID,		sizeof(m_tTimeID));
	stream.w				(&m_tGraphID,		sizeof(m_tGraphID));
	stream.w				(&m_tCombatResult,	sizeof(m_tCombatResult));
	m_tpMonsterGroup1->save	(stream);
	m_tpMonsterGroup2->save	(stream);
}
