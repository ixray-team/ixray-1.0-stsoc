////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_manager.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restrictor_manager.h"
#include "space_restriction.h"
#include "space_restrictor.h"
#include "object_broker.h"

CSpaceRestrictorManager::~CSpaceRestrictorManager		()
{
	delete_data				(m_space_registry);
}

CSpaceRestriction *CSpaceRestrictorManager::restriction	(ref_str space_restrictors)
{
	if (!xr_strlen(space_restrictors))
		return				(0);

	space_restrictors		= normalize(space_restrictors);
	
	SPACE_REGISTRY::const_iterator	I = m_space_registry.find(space_restrictors);
	if (I != m_space_registry.end())
		return				((*I).second);

	CSpaceRestriction		*temp = xr_new<CSpaceRestriction>(space_restrictors);
	m_space_registry.insert	(std::make_pair(space_restrictors,temp));
	return					(temp);
}

void CSpaceRestrictorManager::restrict					(ALife::_OBJECT_ID id, ref_str space_restrictors)
{
	m_clients[id]			= restriction(space_restrictors);
}

ref_str CSpaceRestrictorManager::normalize				(ref_str space_restrictors)
{
	u32						n = _GetItemCount(*space_restrictors);
	if (n < 2)
		return				(space_restrictors);

	m_temp.resize			(n);
	for (u32 i=0; i<n ;++i)
		m_temp[i]			= ref_str(_GetItem(*space_restrictors,i,m_temp_string));

	std::sort				(m_temp.begin(),m_temp.end());

	strcpy					(m_temp_string,"");
	xr_vector<ref_str>::const_iterator	I = m_temp.begin(), B = I;
	xr_vector<ref_str>::const_iterator	E = m_temp.end();
	for ( ; I != E; ++I) {
		if (I != B)
			strcat			(m_temp_string,",");
		strcat				(m_temp_string,**I);
	}

	return					(ref_str(m_temp_string));
}

bool CSpaceRestrictorManager::accessible			(ALife::_OBJECT_ID id, const Fvector &position)
{
	CSpaceRestriction		*_restriction = restriction(id);
	return					(_restriction ? _restriction->inside(position) : true);
}

void CSpaceRestrictorManager::add_restrictor		(CSpaceRestrictor *space_restrictor)
{
	ref_str					space_restrictors = space_restrictor->cName();
	CSpaceRestriction		*temp = xr_new<CSpaceRestriction>(space_restrictor);

	SPACE_REGISTRY::iterator	I = m_space_registry.find(space_restrictors);
	if (I == m_space_registry.end()) {
		m_space_registry.insert	(std::make_pair(space_restrictors,temp));
		return;
	}

	CSpaceRestriction		*old = (*I).second;

	CLIENT_REGISTRY::iterator	i = m_clients.begin();
	CLIENT_REGISTRY::iterator	e = m_clients.end();
	for ( ; i != e; ++i)
		if ((*i).second == old)
			(*i).second		= temp;

	xr_delete				(old);

	m_space_registry.erase	(I);
	m_space_registry.insert	(std::make_pair(space_restrictors,temp));
}

void CSpaceRestrictorManager::add_border			(ALife::_OBJECT_ID id)
{
	CSpaceRestriction		*temp = restriction(id);
	if (temp)
		temp->add_border	();
}

void CSpaceRestrictorManager::remove_border			(ALife::_OBJECT_ID id)
{
	CSpaceRestriction		*temp = restriction(id);
	if (temp)
		temp->remove_border	();
}
