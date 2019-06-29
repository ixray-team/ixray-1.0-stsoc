////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "xrserver_space.h"
#include "alife_space.h"

class CALifeOrganization : public IPureSerializeObject<IReader,IWriter> {
public:
	LPCSTR							m_name;
	LPSTR_VECTOR					m_possible_discoveries;
	float							m_join_probability;
	float							m_left_probability;
	ALife::_ORGANIZATION_ID			m_id;
	ALife::EResearchState			m_research_state;
	ALife::_TIME_ID					m_finish_time;
	ALife::ARTEFACT_ORDER_VECTOR	m_ordered_artefacts;
	ALife::ITEM_COUNT_MAP			m_purchased_artefacts;
	string64						m_discovery_to_investigate;

public:
												CALifeOrganization		();
												CALifeOrganization		(LPCSTR		section);
	virtual										~CALifeOrganization		();
	virtual void								save					(IWriter	&memory_stream);
	virtual void								load					(IReader	&file_stream);
	IC		LPCSTR								name					() const;
	IC		const LPSTR_VECTOR					&possible_discoveries	() const;
	IC		float								join_probability		() const;
	IC		float								left_probability		() const;
	IC		ALife::_ORGANIZATION_ID				id						() const;
	IC		ALife::EResearchState				research_state			() const;
	IC		ALife::_TIME_ID						finish_time				() const;
	IC		const ALife::ARTEFACT_ORDER_VECTOR	&ordered_artefacts		() const;
	IC		const ALife::ITEM_COUNT_MAP			&purchased_artefacts	() const;
	IC		LPCSTR								discovery_to_investigate() const;
};

#include "alife_organization_inline.h"
