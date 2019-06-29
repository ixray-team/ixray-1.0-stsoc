////////////////////////////////////////////////////////////////////////////
//	Module 		: restriction_container.cpp
//	Created 	: 26.08.2004
//  Modified 	: 26.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restriction container
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restriction_container.h"
#include "space_restrictor.h"
#include "object_broker.h"

void CRestrictionContainer::add_restriction		(CSpaceRestrictor *restrictor)
{
	RESTRICTIONS::const_iterator	I = m_restrictions.find(restrictor->ID());
	VERIFY							(I == m_restrictions.end());
	m_restrictions.insert			(std::make_pair(restrictor->ID(),xr_new<RestrictedObject::CShapeRestriction>(restrictor)));
}

CRestrictionContainer::~CRestrictionContainer	()
{
	delete_data						(m_restrictions);
}

