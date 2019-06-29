////////////////////////////////////////////////////////////////////////////
//	Module 		: restriction_container.h
//	Created 	: 26.08.2004
//  Modified 	: 26.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restriction container
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object_space.h"

class CRestrictionContainer {
protected:
	typedef xr_map<u32,RestrictedObject::CInRestriction*>	RESTRICTIONS;

protected:
	RESTRICTIONS		m_restrictions;

private:
	u32					m_id_seed;

public:
						CRestrictionContainer	();
	virtual				~CRestrictionContainer	();
			void		add_restriction			(CSpaceRestrictor *restrictor);
	IC		void		add_restriction			(u32 restriction_id, const Fvector &position, float radius);
	IC		u32			add_restriction			(const Fvector &position, float radius);
	IC		void		remove_restriction		(u32 restriction_id);
	IC		bool		accessible				(const Fvector &position) const;
	IC		bool		empty					() const;
};

#include "restriction_container_inline.h"