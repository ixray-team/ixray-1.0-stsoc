////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_base.cpp
//	Created 	: 20.06.2003
//  Modified 	: 20.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Base evaluation class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ef_base.h"
#include "ai_space.h"
#include "ef_storage.h"

#define EVALUATION_INERTION	2000

bool CBaseFunction::bfCheckForCachedResult()
{
	if ((m_dwLastUpdate + EVALUATION_INERTION >= Device.TimerAsync()) && (m_tpLastMonster == ai().ef_storage().m_tpCurrentMember) && (m_tpLastALifeObject == ai().ef_storage().m_tpCurrentALifeObject))
		return(true);
	m_dwLastUpdate	= Device.TimerAsync();
	m_tpLastMonster = ai().ef_storage().m_tpCurrentMember;
	return(false);
}
