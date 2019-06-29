////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_discovery.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife discovery class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_artefact_demand.h"
#include "alife_space.h"
#include "xrserver_space.h"

class CALifeDiscovery : public IPureSerializeObject<IReader,IWriter> {
public:
	LPCSTR							m_id;
	float							m_success_probability;
	float							m_destroy_probability;
	float							m_result_probability;
	float							m_unfreeze_probability;
	bool							m_invented;
	ALife::DEMAND_P_VECTOR			m_demands;
	ALife::ARTEFACT_ORDER_VECTOR	m_query;
	LPSTR_VECTOR					m_dependency;

public:
									CALifeDiscovery		();
									CALifeDiscovery		(LPCSTR		section);
	virtual							~CALifeDiscovery	();
	virtual void					save				(IWriter	&memory_stream);
	virtual void					load				(IReader	&file_stream);
};

#include "alife_discovery_inline.h"