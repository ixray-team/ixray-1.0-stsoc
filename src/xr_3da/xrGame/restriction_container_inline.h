////////////////////////////////////////////////////////////////////////////
//	Module 		: restriction_container_inline.h
//	Created 	: 26.08.2004
//  Modified 	: 26.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restriction container inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CRestrictionContainer::CRestrictionContainer	()
{
	m_id_seed					= 0xffff;
}

IC	bool CRestrictionContainer::empty				() const
{
	return						(m_restrictions.empty());
}

IC	bool CRestrictionContainer::accessible			(const Fvector &position) const
{
	RESTRICTIONS::const_iterator	I = m_restrictions.begin();
	RESTRICTIONS::const_iterator	E = m_restrictions.end();
	for ( ; I != E; ++I)
		if ((*I).second->accessible(position))
			return					(true);
	return							(false);
}

IC	void CRestrictionContainer::add_restriction		(u32 restriction_id, const Fvector &position, float radius)
{
	RESTRICTIONS::const_iterator	I = m_restrictions.find(restriction_id);
	VERIFY							(I == m_restrictions.end());
	m_restrictions.insert			(std::make_pair(restriction_id,xr_new<RestrictedObject::CDynamicRestriction>(position,radius)));
}

IC	u32	 CRestrictionContainer::add_restriction		(const Fvector &position, float radius)
{
	RESTRICTIONS::const_iterator	I = m_restrictions.find(m_id_seed);
	VERIFY							(I == m_restrictions.end());
	m_restrictions.insert			(std::make_pair(m_id_seed,xr_new<RestrictedObject::CDynamicRestriction>(position,radius)));
	return							(m_id_seed++);
}

IC	void CRestrictionContainer::remove_restriction	(u32 restriction_id)
{
	RESTRICTIONS::iterator			I = m_restrictions.find(restriction_id);
	VERIFY							(I != m_restrictions.end());
	m_restrictions.erase			(I);
}
