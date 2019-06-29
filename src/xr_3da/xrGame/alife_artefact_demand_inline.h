////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_artefact_demand_inline.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife artefact demand class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeArtefactDemand::CALifeArtefactDemand	(
	LPCSTR				section, 
	u32					min_count, 
	u32					max_count, 
	u32					min_price, 
	u32					max_price
) :	
	m_section			(section),
	m_min_count			(min_count),
	m_max_count			(max_count),
	m_min_price			(min_price),
	m_max_price			(max_price)
{
}

IC	u32	CALifeArtefactDemand::min_count	() const
{
	return				(m_min_count);
}

IC	u32	CALifeArtefactDemand::max_count	() const
{
	return				(m_max_count);
}

IC	u32	CALifeArtefactDemand::min_price	() const
{
	return				(m_min_price);
}

IC	u32	CALifeArtefactDemand::max_price	() const
{
	return				(m_max_price);
}
