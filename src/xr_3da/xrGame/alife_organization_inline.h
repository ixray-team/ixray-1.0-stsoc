////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization_inline.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	LPCSTR								CALifeOrganization::name					() const
{
	return				(m_name);
}

IC	const LPSTR_VECTOR					&CALifeOrganization::possible_discoveries	() const
{
	return				(m_possible_discoveries);
}

IC	float								CALifeOrganization::join_probability		() const
{
	return				(m_join_probability);
}

IC	float								CALifeOrganization::left_probability		() const
{
	return				(m_left_probability);
}

IC	ALife::_ORGANIZATION_ID				CALifeOrganization::id						() const
{
	return				(m_id);
}

IC	ALife::EResearchState				CALifeOrganization::research_state			() const
{
	return				(m_research_state);
}

IC	ALife::_TIME_ID						CALifeOrganization::finish_time				() const
{
	return				(m_finish_time);
}

IC	const ALife::ARTEFACT_ORDER_VECTOR	&CALifeOrganization::ordered_artefacts		() const
{
	return				(m_ordered_artefacts);
}

IC	const ALife::ITEM_COUNT_MAP			&CALifeOrganization::purchased_artefacts	() const
{
	return				(m_purchased_artefacts);
}

IC	LPCSTR								CALifeOrganization::discovery_to_investigate() const
{
	return				(m_discovery_to_investigate);
}
