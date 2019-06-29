////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_manager.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSpaceRestriction;
class CSpaceRestrictor;

class CSpaceRestrictorManager {
public:
	typedef xr_map<ref_str,CSpaceRestriction*>				SPACE_REGISTRY;
	typedef xr_map<ALife::_OBJECT_ID,CSpaceRestriction*>	CLIENT_REGISTRY;

private:
	xr_vector<ref_str>				m_temp;
	string256						m_temp_string;

protected:
	SPACE_REGISTRY					m_space_registry;
	CLIENT_REGISTRY					m_clients;

protected:
			ref_str					normalize					(ref_str space_restrictors);

public:
	IC								CSpaceRestrictorManager		();
	virtual							~CSpaceRestrictorManager	();
			void					restrict					(ALife::_OBJECT_ID id, ref_str space_restrictors);
			bool					accessible					(ALife::_OBJECT_ID id, const Fvector &position);
			CSpaceRestriction		*restriction				(ref_str space_restrictors);
	IC		CSpaceRestriction		*restriction				(ALife::_OBJECT_ID id);
			void					add_restrictor				(CSpaceRestrictor *space_restrictor);
	IC		const SPACE_REGISTRY	&restrictions				() const;
			void					add_border					(ALife::_OBJECT_ID id);
			void					remove_border				(ALife::_OBJECT_ID id);
};

#include "space_restrictor_manager_inline.h"