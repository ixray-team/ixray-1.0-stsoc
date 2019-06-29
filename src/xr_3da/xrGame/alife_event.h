////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"
#include "game_graph_space.h"

class CALifeEventGroup;

class CALifeEvent : public IPureSerializeObject<IReader,IWriter> {
public:
	ALife::_EVENT_ID		m_tEventID;
	ALife::_TIME_ID			m_tTimeID;
	GameGraph::_GRAPH_ID	m_tGraphID;
	ALife::ECombatResult	m_tCombatResult;
	CALifeEventGroup		*m_tpMonsterGroup1;
	CALifeEventGroup		*m_tpMonsterGroup2;

public:
	virtual					~CALifeEvent			();
	virtual void			load					(IReader &stream);
	virtual void			save					(IWriter &stream);
};

#include "alife_event_inline.h"