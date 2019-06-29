////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSE_ALifeTrader;

class CALifeOrganizationRegistry : CRandom {
protected:
	ALife::ORGANIZATION_P_MAP				m_organizations;
	ALife::DISCOVERY_P_MAP					m_discoveries;
	ALife::LPSTR_BOOL_MAP					m_artefacts;
	ALife::TRADER_SET_MAP					m_cross_traders;

public:
											CALifeOrganizationRegistry	(LPCSTR section);
	virtual									~CALifeOrganizationRegistry	();
	virtual	void							save						(IWriter &memory_stream);
	virtual	void							load						(IReader &file_stream);
			void							update						();
			void							update_artefact_orders		(CSE_ALifeTrader &trader);
	IC		const ALife::ORGANIZATION_P_MAP &organizations				() const;
	IC		CALifeDiscovery					*discovery					(LPCSTR discovery_name, bool no_assert = false) const;
	IC		const ALife::TRADER_SET_MAP		&cross_traders				() const;
};

#include "alife_organization_registry_inline.h"