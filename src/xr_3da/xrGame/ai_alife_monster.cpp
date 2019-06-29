////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_monster.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life mnsters simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"
#include "game_graph.h"
#include "ef_storage.h"
using namespace ALife;

void CSE_ALifeMonsterAbstract::Update		()
{
	bool				bContinue = true;
	while (bContinue && bfActive() && (m_tpALife->m_tpActor->o_Position.distance_to(o_Position) > m_tpALife->m_fSwitchDistance)) {
		vfCheckForPopulationChanges();
		bContinue		= false;
		if (m_tNextGraphID != m_tGraphID) {
			_TIME_ID					tCurTime = m_tpALife->tfGetGameTime();
			m_fDistanceFromPoint		+= float(tCurTime - m_tTimeID)/1000.f/m_tpALife->normal_time_factor()*m_fCurSpeed;
			if (m_fDistanceToPoint - m_fDistanceFromPoint < EPS_L) {
				bContinue = true;
				if ((m_fDistanceFromPoint - m_fDistanceToPoint > EPS_L) && (m_fCurSpeed > EPS_L))
					m_tTimeID			= tCurTime - _TIME_ID(iFloor((m_fDistanceFromPoint - m_fDistanceToPoint)*1000.f/m_fCurSpeed));
				m_fDistanceToPoint		= m_fDistanceFromPoint	= 0.0f;
				m_tPrevGraphID			= m_tGraphID;
				m_tpALife->vfChangeObjectGraphPoint(this,m_tGraphID,m_tNextGraphID);
				CSE_ALifeGroupAbstract	*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
				if (tpALifeGroupAbstract)
					tpALifeGroupAbstract->m_bCreateSpawnPositions = true;
			}
		}
		if (m_tNextGraphID == m_tGraphID) {
			_GRAPH_ID			tGraphID = m_tNextGraphID;
			CGameGraph::const_iterator	i,e;
			TERRAIN_VECTOR		&tpaTerrain = m_tpaTerrain;
			int							iPointCount = (int)tpaTerrain.size();
			int							iBranches = 0;
			ai().game_graph().begin		(tGraphID,i,e);
			for ( ; i != e; ++i)
				if ((*i).vertex_id() != m_tPrevGraphID)
					for (int j=0; j<iPointCount; ++j)
						if (ai().game_graph().mask(tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()))
							++iBranches;
			bool						bOk = false;
			ai().game_graph().begin		(tGraphID,i,e);
			if (!iBranches) {
				for ( ; i != e; ++i) {
					for (int j=0; j<iPointCount; ++j)
						if (ai().game_graph().mask(tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type())) {
							m_tNextGraphID = (*i).vertex_id();
							m_fDistanceToPoint = (*i).distance();
							bOk = true;
							break;
						}
						if (bOk)
							break;
				}
			}
			else {
				int iChosenBranch = m_tpALife->randI(0,iBranches);
				iBranches = 0;
				for ( ; i != e; ++i)
					if ((*i).vertex_id() != m_tPrevGraphID) {
						for (int j=0; j<iPointCount; ++j)
							if (ai().game_graph().mask(tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tPrevGraphID)) {
								if (iBranches == iChosenBranch) {
									m_tNextGraphID	= (*i).vertex_id();
									m_fDistanceToPoint = (*i).distance();
									bOk = true;
									break;
								}
								++iBranches;
							}
							if (bOk)
								break;
					}
			}
			if (!bOk) {
				m_fCurSpeed			= 0.0f;
				m_fDistanceToPoint	= 0.0f;
				bContinue			= false;
			}
			else
				m_fCurSpeed			= m_fGoingSpeed;
		}
		m_tpALife->vfCheckForInteraction(this);
	}
	m_tTimeID					= m_tpALife->tfGetGameTime();
}

CSE_ALifeItemWeapon	*CSE_ALifeMonsterAbstract::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	fHitPower					= m_fHitPower;
	tHitType					= m_tHitType;
	return						(m_tpCurrentBestWeapon);
}

EMeetActionType	CSE_ALifeMonsterAbstract::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	if (eCombatTypeMonsterMonster == m_tpALife->m_tCombatType) {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Inconsistent meet action type");
		return						(eRelationTypeFriend == m_tpALife->tfGetRelationType(this,dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable)) ? eMeetActionTypeIgnore : ((bMutualDetection || m_tpALife->tfChooseCombatAction(iGroupIndex) == eCombatActionAttack) ? eMeetActionTypeAttack : eMeetActionTypeIgnore));
	}
	else
		return(eMeetActionTypeAttack);
}

bool CSE_ALifeMonsterAbstract::bfActive()
{
	CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
	return						((l_tpALifeGroupAbstract && (l_tpALifeGroupAbstract->m_wCount > 0)) || (!l_tpALifeGroupAbstract && (fHealth > EPS_L)));
}

CSE_ALifeDynamicObject *CSE_ALifeMonsterAbstract::tpfGetBestDetector()
{
	CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
	if (!l_tpALifeGroupAbstract)
		return					(this);
	else {
		if (!l_tpALifeGroupAbstract->m_wCount)
			return				(0);
		else
			return				(m_tpALife->object(l_tpALifeGroupAbstract->m_tpMembers[0]));
	}
}

void CSE_ALifeMonsterAbstract::vfCheckForPopulationChanges()
{
	CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
	if (!l_tpALifeGroupAbstract || !bfActive() || m_bOnline)
		return;

	_TIME_ID					l_tTimeID = m_tpALife->tfGetGameTime();
	if (l_tTimeID >= l_tpALifeGroupAbstract->m_tNextBirthTime) {
		ai().ef_storage().m_tpCurrentALifeMember = this;
		l_tpALifeGroupAbstract->m_tNextBirthTime = l_tTimeID + _TIME_ID(ai().ef_storage().m_pfBirthSpeed->ffGetValue()*24*60*60*1000);
		if (m_tpALife->randF(100) < ai().ef_storage().m_pfBirthProbability->ffGetValue()) {
			u32					l_dwBornCount = iFloor(float(l_tpALifeGroupAbstract->m_wCount)*m_tpALife->randF(.5f,1.5f)*ai().ef_storage().m_pfBirthPercentage->ffGetValue()/100.f + .5f);
			if (l_dwBornCount) {
				l_tpALifeGroupAbstract->m_tpMembers.resize(l_tpALifeGroupAbstract->m_wCount + l_dwBornCount);
				OBJECT_IT		I = l_tpALifeGroupAbstract->m_tpMembers.begin() + l_tpALifeGroupAbstract->m_wCount;
				OBJECT_IT		E = l_tpALifeGroupAbstract->m_tpMembers.end();
				for ( ; I != E; ++I) {
					CSE_Abstract		*l_tpAbstract = m_tpALife->tpfCreateGroupMember	(l_tpALifeGroupAbstract,this);
					*I					= l_tpAbstract->ID;
				}
				l_tpALifeGroupAbstract->m_wCount = l_tpALifeGroupAbstract->m_wCount + u16(l_dwBornCount);
			}
		}
	}
}