////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object_space.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictor;

namespace RestrictedObject {
	enum ERestrictionType {
		eRestrictionTypeIn		= u32(0),
		eRestrictionTypeOut,
		eRestrictionTypeDummy	= u32(-1),
	};

	class CInRestriction {
	private:
		bool			m_enabled;

	public:
		virtual bool	accessible				(const Fvector &position) const = 0;
		IC		void	enable					(bool value) {m_enabled = value;}
		IC		bool	enabled					() const {return m_enabled;}
	};

	class CShapeRestriction : public CInRestriction {
	private:
		CSpaceRestrictor	*m_restrictor;

	public:
		IC				CShapeRestriction		(CSpaceRestrictor *restrictor)
		{
			m_restrictor	= restrictor;
			VERIFY			(m_restrictor);
		}

		virtual	bool	accessible				(const Fvector &position) const;
	};

	class CDynamicRestriction  : public CInRestriction {
	private:
		Fvector				m_position;
		float				m_radius_sqr;

	public:
		IC				CDynamicRestriction		(const Fvector &position, float radius)
		{
			m_position		= position;
			m_radius_sqr	= _sqr(radius);
		}

		IC	virtual	bool	accessible			(const Fvector &position) const
		{
			return			(m_position.distance_to_sqr(position) > m_radius_sqr);
		}
	};

};

