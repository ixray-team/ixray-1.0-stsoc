////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_hit.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "ai_script_bind.h"

class CLuaHit {
public:
	float				m_fPower; 
	Fvector				m_tDirection;
	ref_str				m_caBoneName;
	CLuaGameObject		*m_tpDraftsman;
	float				m_fImpulse;
	int					m_tHitType;

							CLuaHit				()
	{
		m_fPower		= 100;
		m_tDirection.set(1,0,0);
		m_caBoneName	= "";
		m_tpDraftsman	= 0;
		m_fImpulse		= 100;
		m_tHitType		= ALife::eHitTypeWound;
	}

							CLuaHit				(const CLuaHit *tpLuaHit)
	{
		*this			= *tpLuaHit;
	}

	virtual					~CLuaHit			()
	{
	}

			void			set_bone_name		(LPCSTR bone_name)
	{
		m_caBoneName	= bone_name;
	}
};

