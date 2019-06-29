////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_artefact_demand.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife artefact demand class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CALifeArtefactDemand {
public:
	LPCSTR			m_section;
	u32				m_min_count;
	u32				m_max_count;
	u32				m_min_price;
	u32				m_max_price;

public:
	IC				CALifeArtefactDemand	(LPCSTR section, u32 min_count, u32 max_count, u32 min_price, u32 max_price);
	IC		u32		min_count				() const;
	IC		u32		max_count				() const;
	IC		u32		min_price				() const;
	IC		u32		max_price				() const;
};

#include "alife_artefact_demand_inline.h"