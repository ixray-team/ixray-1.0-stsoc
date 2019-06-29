////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_effector.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "ai_script_bind.h"
#include "../effectorpp.h"
#include "level.h"

class CLuaEffector : public CEffectorPP {
public:
	typedef CEffectorPP inherited;
	EEffectorPPType		m_tEffectorType;
	SPPInfo				m_tInfo;

					CLuaEffector				(int		iType, float time) : CEffectorPP(EEffectorPPType(iType),time,false)
	{
		m_tEffectorType		= EEffectorPPType(iType);
	}

	virtual			~CLuaEffector				()
	{
	}

	virtual	BOOL	Process						(SPPInfo	&pp)
	{
		return		(inherited::Process(pp));
	}

	virtual	void	Add							()
	{
		Level().Cameras.AddEffector		(this);
	}

	virtual	void	Remove							()
	{
		Level().Cameras.RemoveEffector	(m_tEffectorType);
	}
};

class CLuaEffectorWrapper : public CLuaEffector, public luabind::wrap_base {
public:
					CLuaEffectorWrapper	(int iType, float fTime) : CLuaEffector(iType, fTime)
	{
	}

	virtual BOOL	Process				(SPPInfo &pp)
	{
		BOOL	l_bResult = !!call_member<bool>("process",pp);
		pp		= m_tInfo;
		return	(l_bResult);
	}

	static	bool	Process_static		(CLuaEffector *tpLuaEffector, SPPInfo &pp)
	{
		return		(!!tpLuaEffector->CLuaEffector::Process(pp));
	}
};

