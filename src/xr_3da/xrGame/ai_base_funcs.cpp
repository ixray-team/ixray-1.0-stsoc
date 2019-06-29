////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_base_funcs.cpp
//	Created 	: 20.06.2003
//  Modified 	: 20.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Base function classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_base_funcs.h"
#include "ai_space.h"

#define EVALUATION_INERTION	2000

bool CBaseFunction::bfCheckForCachedResult()
{
	if ((m_dwLastUpdate + EVALUATION_INERTION >= Device.TimerAsync()) && (m_tpLastMonster == getAI().m_tpCurrentMember) && (m_tpLastALifeObject == getAI().m_tpCurrentALifeObject))
		return(true);
	m_dwLastUpdate	= Device.TimerAsync();
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(false);
}
