////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_zone.cpp
//	Created 	: 19.08.2003
//  Modified 	: 19.08.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life zones simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

void CSE_ALifeAnomalousZone::Update()
{
	R_ASSERT3					(false,"This function shouldn't be called!",s_name_replace);
//	m_maxPower					= m_fStartPower*(m_tpALife->m_tNextSurgeTime - m_tTimeID)/(m_tpALife->m_tNextSurgeTime - m_tpALife->m_tLastSurgeTime);
//	m_tpALife->vfCheckForInteraction(this);
}

CSE_ALifeItemWeapon	*CSE_ALifeAnomalousZone::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	m_tTimeID					= m_tpALife->tfGetGameTime();
	m_maxPower					= m_fStartPower*(m_tpALife->m_tNextSurgeTime - m_tTimeID)/(m_tpALife->m_tNextSurgeTime - m_tpALife->m_tLastSurgeTime);
	fHitPower					= m_maxPower;
	tHitType					= m_tHitType;
	return						(m_tpCurrentBestWeapon);
}

EMeetActionType	CSE_ALifeAnomalousZone::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	return						(eMeetActionTypeAttack);
}

bool CSE_ALifeAnomalousZone::bfActive()
{
	return						(m_maxPower > EPS_L);
}

CSE_ALifeDynamicObject *CSE_ALifeAnomalousZone::tpfGetBestDetector()
{
	VERIFY2						(false,"This function shouldn't be called");
	NODEFAULT;
#ifdef DEBUG
	return						(0);
#endif
}
