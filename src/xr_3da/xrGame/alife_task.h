////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"
#include "game_graph_space.h"

class CALifeTask : public IPureSerializeObject<IReader,IWriter> {
public:
	ALife::_TASK_ID				m_tTaskID;
	ALife::_TIME_ID				m_tTimeID;
	ALife::_OBJECT_ID			m_tCustomerID;
	float						m_fCost;
	ALife::ETaskType			m_tTaskType;
	u32							m_dwTryCount;
	union {
		string32				m_caSection;
		ALife::_OBJECT_ID		m_tObjectID;
	};
	union {
		GameGraph::_LOCATION_ID	m_tLocationID[GameGraph::LOCATION_TYPE_COUNT];
		GameGraph::_GRAPH_ID	m_tGraphID;
	};

public:
	IC							CALifeTask		();
	virtual						~CALifeTask		();
	virtual void				load			(IReader &stream);
	virtual void				save			(IWriter &stream);
};

#include "alife_task_inline.h"