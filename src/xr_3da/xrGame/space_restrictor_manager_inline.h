////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_manager_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictorManager::CSpaceRestrictorManager		()
{
}

IC	CSpaceRestriction *CSpaceRestrictorManager::restriction	(ALife::_OBJECT_ID id)
{
	CLIENT_REGISTRY::iterator	I = m_clients.find(id);
	VERIFY						(m_clients.end() != I);
	return						((*I).second);
}

IC	const CSpaceRestrictorManager::SPACE_REGISTRY &CSpaceRestrictorManager::restrictions	() const
{
	return						(m_space_registry);
}
