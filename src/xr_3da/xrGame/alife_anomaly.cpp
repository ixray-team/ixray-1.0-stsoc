////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_zone.cpp
//	Created 	: 19.08.2003
//  Modified 	: 19.08.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life zones simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "alife_graph_registry.h"
#include "alife_spawn_registry.h"
#include "ai_space.h"
#include "monster_community.h"

using namespace ALife;

void CSE_ALifeAnomalousZone::update()
{
//	R_ASSERT3					(false,"This function shouldn't be called!",s_name_replace);
	m_maxPower					= m_fStartPower*(ai().alife().time_manager().next_surge_time() - m_tTimeID)/(ai().alife().time_manager().next_surge_time() - ai().alife().time_manager().last_surge_time());
//	ai().alife().vfCheckForInteraction(this);
}

CSE_ALifeItemWeapon	*CSE_ALifeAnomalousZone::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	m_tTimeID					= ai().alife().time_manager().game_time();
	m_maxPower					= m_fStartPower*(ai().alife().time_manager().next_surge_time() - m_tTimeID)/(ai().alife().time_manager().next_surge_time() - ai().alife().time_manager().last_surge_time());
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
	return						(fis_zero(m_maxPower,EPS_L) || !interactive());
}

CSE_ALifeDynamicObject *CSE_ALifeAnomalousZone::tpfGetBestDetector()
{
	VERIFY2						(false,"This function shouldn't be called");
	NODEFAULT;
#ifdef DEBUG
	return						(0);
#endif
}

CSE_ALifeItemWeapon	*CSE_ALifeSmartZone::tpfGetBestWeapon	(ALife::EHitType		&tHitType,			float		&fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	return						(m_tpCurrentBestWeapon);
}

ALife::EMeetActionType CSE_ALifeSmartZone::tfGetActionType	(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection)
{
	CSE_ALifeObject				*object = smart_cast<CSE_ALifeObject*>(tpALifeSchedulable->base());
	VERIFY						(object);
	return						((object->m_tGraphID == m_tGraphID) ? ALife::eMeetActionSmartTerrain : ALife::eMeetActionTypeIgnore);
}

bool CSE_ALifeSmartZone::bfActive							()
{
	return						(true);
}

CSE_ALifeDynamicObject *CSE_ALifeSmartZone::tpfGetBestDetector	()
{
	VERIFY2						(false,"This function shouldn't be called");
	return						(0);
}

void CSE_ALifeDynamicObject::on_spawn		()
{
}

void CSE_ALifeCreatureAbstract::on_spawn	()
{
	inherited::on_spawn					();
	m_dynamic_out_restrictions.clear	();
	m_dynamic_in_restrictions.clear		();

	if (smart_cast<CSE_ALifeGroupAbstract*>(this))
		return;

	MONSTER_COMMUNITY monster_community;
	monster_community.set	(pSettings->r_string(s_name, "species"));
	if(monster_community.team() != 255)
		s_team = monster_community.team();
}

void CSE_ALifeAnomalousZone::on_spawn						()
{
	inherited1::on_spawn	();

	m_maxPower				= m_fStartPower = randF(m_min_start_power,m_max_start_power);
	u32						jj = iFloor(randF(m_fBirthProbability)*m_maxPower/m_power_artefact_factor);

	for (u32 ii=0; ii<jj; ++ii) {
		float fProbability		= randF(1.f);
		float fSum				= 0.f;
		for (u16 p=0; p<m_wItemCount; ++p) {
			fSum			+= m_faWeights[p];
			if (fSum > fProbability)
				break;
		}
		if (p < m_wItemCount) {
			CSE_Abstract	*l_tpSE_Abstract = alife().spawn_item(m_cppArtefactSections[p],position(),m_tNodeID,m_tGraphID,0xffff);
			R_ASSERT3		(l_tpSE_Abstract,"Can't spawn artefact ",m_cppArtefactSections[p]);
			CSE_ALifeDynamicObject	*i = smart_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
			R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");

			i->m_tSpawnID		= m_tSpawnID;
			i->m_bALifeControl	= true;
			ai().alife().spawns().assign_artefact_position(this,i);

			Fvector				t = i->o_Position	;
			u32					p = i->m_tNodeID	;
			float				q = i->m_fDistance	;
			alife().graph().change(i,m_tGraphID,i->m_tGraphID);
			i->o_Position		= t;
			i->m_tNodeID		= p;
			i->m_fDistance		= q;

			CSE_ALifeItemArtefact *l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(i);
			R_ASSERT2		(l_tpALifeItemArtefact,"Anomalous zone can't generate non-artefact objects since they don't have an 'anomaly property'!");

			l_tpALifeItemArtefact->m_fAnomalyValue = m_maxPower*(1.f - i->o_Position.distance_to(o_Position)/m_fRadius);
		}
	}
}

void CSE_ALifeMonsterAbstract::on_surge	()
{
	inherited1::on_surge			();
	m_dynamic_out_restrictions.clear();
	m_dynamic_in_restrictions.clear	();
}
