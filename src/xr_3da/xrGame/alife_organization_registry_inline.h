////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const ALife::ORGANIZATION_P_MAP &CALifeOrganizationRegistry::organizations	() const
{
	return					(m_organizations);
}

IC	CALifeDiscovery	*CALifeOrganizationRegistry::discovery	(LPCSTR discovery_name, bool no_assert) const
{
	ALife::DISCOVERY_P_MAP::const_iterator	I = m_discoveries.find(discovery_name);
	if (I == m_discoveries.end()) {
		THROW3				(no_assert,"Cannot find ther specified discovery",discovery_name);
		return				(0);
	}
	return					((*I).second);
}

IC	const ALife::TRADER_SET_MAP	&CALifeOrganizationRegistry::cross_traders	() const
{
	return					(m_cross_traders);
}
