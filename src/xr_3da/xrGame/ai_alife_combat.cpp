////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_combat.cpp
//	Created 	: 12.08.2003
//  Modified 	: 12.08.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life combats simulation 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ef_storage.h"
#include "game_graph.h"
#include "level_graph.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

void CSE_ALifeSimulator::vfFillCombatGroup(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex)
{
	EHitType				l_tHitType;
	float					l_fHitPower;
	SCHEDULE_P_VECTOR		&tpGroupVector = m_tpaCombatGroups[iGroupIndex];
	CSE_ALifeGroupAbstract	*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(tpALifeSchedulable);
	tpGroupVector.clear		();
	if (l_tpALifeGroupAbstract) {
		OBJECT_IT			I = l_tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT			E = l_tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; ++I) {
			CSE_ALifeSchedulable	*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(object(*I));
			R_ASSERT2				(l_tpALifeSchedulable,"Invalid combat object");
			tpGroupVector.push_back(l_tpALifeSchedulable);
			l_tpALifeSchedulable->tpfGetBestWeapon(l_tHitType,l_fHitPower);
		}
	}
	else {
		tpGroupVector.push_back(tpALifeSchedulable);
		tpALifeSchedulable->tpfGetBestWeapon(l_tHitType,l_fHitPower);
	}
	m_tpaCombatObjects[iGroupIndex] = tpALifeSchedulable;
}

ECombatAction CSE_ALifeSimulator::tfChooseCombatAction(int iCombatGroupIndex)
{
	if (eCombatTypeMonsterMonster != m_tCombatType)
		if (((eCombatTypeAnomalyMonster == m_tCombatType) && !iCombatGroupIndex) || ((eCombatTypeMonsterAnomaly == m_tCombatType) && iCombatGroupIndex))
			return(eCombatActionAttack);
		else
			return(eCombatActionRetreat);

	SCHEDULE_P_VECTOR	&Members = m_tpaCombatGroups[iCombatGroupIndex];
	SCHEDULE_P_VECTOR	&Enemies = m_tpaCombatGroups[iCombatGroupIndex ^ 1];
	int i = 0, j = 0, I = (int)Members.size(), J = (int)Enemies.size();
	float	fMinProbability;
	if (!I)
		fMinProbability = 0;
	else {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[0]);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid combat object");
		fMinProbability				= l_tpALifeMonsterAbstract->m_fRetreatThreshold;
	}
	while ((i < I) && (j < J)) {
		ai().ef_storage().m_tpCurrentALifeMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[i]);
		ai().ef_storage().m_tpCurrentALifeEnemy	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Enemies[j]);
		float fProbability = ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f, fCurrentProbability;
		if (fProbability > fMinProbability) {
			fCurrentProbability = fProbability;
			for (++j; (i < I) && (j < J); ++j) {
				ai().ef_storage().m_tpCurrentALifeEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>(Enemies[j]);
				fProbability = ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
					++i;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
		else {
			fCurrentProbability = 1.0f - fProbability;
			for (++i; (i < I) && (j < J); ++i) {
				ai().ef_storage().m_tpCurrentALifeMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[i]);
				fProbability = 1.0f - ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
					++j;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
	}
	return((j >= J) ? eCombatActionAttack : eCombatActionRetreat);
}

bool CSE_ALifeSimulator::bfCheckObjectDetection(CSE_ALifeSchedulable *tpALifeSchedulable1, CSE_ALifeSchedulable *tpALifeSchedulable2)
{
	switch (m_tCombatType) {
		case eCombatTypeMonsterMonster : {
			ai().ef_storage().m_tpCurrentALifeObject	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable1);
			ai().ef_storage().m_tpCurrentALifeMember	= tpALifeSchedulable1;
			ai().ef_storage().m_tpCurrentALifeEnemy		= tpALifeSchedulable2;
			return										(randF(100) < (int)ai().ef_storage().m_pfEnemyDetectProbability->ffGetValue());
		}
		case eCombatTypeAnomalyMonster : {
			ai().ef_storage().m_tpCurrentALifeEnemy		= tpALifeSchedulable1;
			return										(randF(100) < (int)ai().ef_storage().m_pfAnomalyInteractProbability->ffGetValue());
		}
		case eCombatTypeMonsterAnomaly : {
			ai().ef_storage().m_tpCurrentALifeObject	= tpALifeSchedulable1->tpfGetBestDetector();
			ai().ef_storage().m_tpCurrentALifeMember	= tpALifeSchedulable1;
			ai().ef_storage().m_tpCurrentALifeEnemy		= tpALifeSchedulable2;
			return										(randF(100) < (int)ai().ef_storage().m_pfAnomalyDetectProbability->ffGetValue());
		}
		default :							NODEFAULT;
	}
	return									(false);
}

bool CSE_ALifeSimulator::bfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable1, CSE_ALifeSchedulable *tpALifeSchedulable2, int &iCombatGroupIndex, bool &bMutualDetection)
{
	if (!tpALifeSchedulable1->bfActive() || !tpALifeSchedulable2->bfActive())
		return(false);
	
	// determine combat type
	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract1	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable1);
	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract2	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable2);
	if (!l_tpALifeMonsterAbstract1) {
		if (!l_tpALifeMonsterAbstract2)
			return(false);
		else {
			CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable1);
			R_ASSERT2				(l_tpALifeAnomalousZone,"Unknown schedulable object class");
			m_tCombatType			= eCombatTypeAnomalyMonster;
		}
	}
	else {
		if (!l_tpALifeMonsterAbstract2) {
			CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable2);
			R_ASSERT2				(l_tpALifeAnomalousZone,"Unknown schedulable object class");
			m_tCombatType			= eCombatTypeMonsterAnomaly;
		}
		else {
			m_tCombatType			= eCombatTypeMonsterMonster;
			if (eRelationTypeFriend == tfGetRelationType(l_tpALifeMonsterAbstract1,l_tpALifeMonsterAbstract2)) {
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract1 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeMonsterAbstract1);
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract2 = dynamic_cast<CSE_ALifeHumanAbstract*>(l_tpALifeMonsterAbstract2);
				if (l_tpALifeHumanAbstract1 && l_tpALifeHumanAbstract2) {
					iCombatGroupIndex = 0;
					return(true);
				}
				else
					return(false);
			}
		}
	}
	
	// perform interaction
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		_GRAPH_ID						l_tGraphID = l_tpALifeMonsterAbstract1 ? l_tpALifeMonsterAbstract1->m_tGraphID : l_tpALifeMonsterAbstract2->m_tGraphID;
		vfPrintTime						("\n[LSS]",tfGetGameTime());
		Msg								("[LSS] %s met %s on the graph point %d (level %s[%d][%d][%d][%d])",
			tpALifeSchedulable1->s_name_replace,
			tpALifeSchedulable2->s_name_replace,
			l_tGraphID,
			ai().game_graph().header().levels().find(ai().game_graph().vertex(l_tGraphID)->level_id())->second.name(),
			ai().game_graph().vertex(l_tGraphID)->vertex_type()[0],
			ai().game_graph().vertex(l_tGraphID)->vertex_type()[1],
			ai().game_graph().vertex(l_tGraphID)->vertex_type()[2],
			ai().game_graph().vertex(l_tGraphID)->vertex_type()[3]
		);
	}
#endif
	
	bMutualDetection				= false;
	iCombatGroupIndex				= -1;

	if (bfCheckObjectDetection(tpALifeSchedulable1,tpALifeSchedulable2)) {
		iCombatGroupIndex			= 0;
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg						("[LSS] %s detected %s",tpALifeSchedulable1->s_name_replace,tpALifeSchedulable2->s_name_replace);
		}
#endif
	}
	else {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg						("[LSS] %s didn't detect %s",tpALifeSchedulable1->s_name_replace,tpALifeSchedulable2->s_name_replace);
		}
#endif
	}

	if (eCombatTypeMonsterAnomaly == m_tCombatType)
		m_tCombatType = eCombatTypeAnomalyMonster;
	else
		if (eCombatTypeAnomalyMonster == m_tCombatType)
			m_tCombatType = eCombatTypeMonsterAnomaly;

	if (bfCheckObjectDetection(tpALifeSchedulable2,tpALifeSchedulable1)) {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg						("[LSS] %s detected %s",tpALifeSchedulable2->s_name_replace,tpALifeSchedulable1->s_name_replace);
		}
#endif
		if (!iCombatGroupIndex)
			bMutualDetection		= true;
		else
			iCombatGroupIndex		= 1;
	}
	else {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg						("[LSS] %s didn't detect %s",tpALifeSchedulable2->s_name_replace,tpALifeSchedulable1->s_name_replace);
		}
#endif
	}

	if (eCombatTypeMonsterAnomaly == m_tCombatType)
		m_tCombatType = eCombatTypeAnomalyMonster;
	else
		if (eCombatTypeAnomalyMonster == m_tCombatType)
			m_tCombatType = eCombatTypeMonsterAnomaly;

	if (iCombatGroupIndex < 0) {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg						("[LSS] There is no interaction");
		}
#endif
		return						(false);
	}
	else
		return						(true);
}

bool CSE_ALifeSimulator::bfCheckIfRetreated(int iCombatGroupIndex)
{
	ai().ef_storage().m_tpCurrentALifeObject	= (eCombatTypeMonsterMonster == m_tCombatType) ? dynamic_cast<CSE_ALifeObject*>(m_tpaCombatGroups[iCombatGroupIndex][0]) : m_tpaCombatGroups[iCombatGroupIndex][0]->m_tpBestDetector;
	ai().ef_storage().m_tpCurrentALifeMember	= m_tpaCombatGroups[iCombatGroupIndex][0];
	ai().ef_storage().m_tpCurrentALifeEnemy		= m_tpaCombatGroups[iCombatGroupIndex ^ 1][0];
	return										(randF(100) < ((eCombatTypeMonsterMonster != m_tCombatType) ? ai().ef_storage().m_pfAnomalyRetreatProbability->ffGetValue() : ai().ef_storage().m_pfEnemyRetreatProbability->ffGetValue()));
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iCombatGroupIndex)
{
	SCHEDULE_P_VECTOR		&l_tCombatGroup = m_tpaCombatGroups[iCombatGroupIndex];
	SCHEDULE_P_IT			I = l_tCombatGroup.begin();
	SCHEDULE_P_IT			E = l_tCombatGroup.end();
	for ( ; I != E; ++I) {
		EHitType			l_tHitType = eHitTypeMax;
		float				l_fHitPower = 0.f;
		if (!(*I)->m_tpCurrentBestWeapon) {
			CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = (*I)->tpfGetBestWeapon(l_tHitType,l_fHitPower);
			if (!l_tpALifeItemWeapon && (l_fHitPower <= EPS_L))
				continue;
		}
		else {
			l_tHitType		= (*I)->m_tpCurrentBestWeapon->m_tHitType;
			l_fHitPower		= (*I)->m_tpCurrentBestWeapon->m_fHitPower;
		}
		
		ai().ef_storage().m_tpCurrentALifeObject = dynamic_cast<CSE_ALifeObject*>(*I);
		ai().ef_storage().m_tpCurrentALifeMember = *I;
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg				("[LSS] %s attacks with %s(%d ammo) %d times in a row",(*I)->s_name_replace,(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->s_name_replace : "its natural weapon",(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->m_dwAmmoAvailable : 0,iFloor(ai().ef_storage().m_pfWeaponAttackTimes->ffGetValue() + .5f));
		}
#endif
		for (int i=0, n=iFloor(ai().ef_storage().m_pfWeaponAttackTimes->ffGetValue() + .5f); i<n; ++i) {
			if (randF(100) < (int)ai().ef_storage().m_pfWeaponSuccessProbability->ffGetValue()) {
				// choose random enemy group member and perform hit with random power
				// multiplied by immunity factor
				int							l_iIndex = randI(m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(m_tpaCombatGroups[iCombatGroupIndex ^ 1][l_iIndex]);
				R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid combat object");
				float						l_fHit = randF(l_fHitPower*0.5f,l_fHitPower*1.5f);
				l_tpALifeMonsterAbstract->fHealth -= l_tpALifeMonsterAbstract->m_fpImmunityFactors[l_tHitType]*l_fHit;
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg						("[LSS] %s %s %s [power %5.2f][damage %5.2f][health %5.2f][creatures left %d]",(*I)->s_name_replace,l_tpALifeMonsterAbstract->fHealth <= 0 ? "killed" : "attacked",l_tpALifeMonsterAbstract->s_name_replace,l_fHit,l_tpALifeMonsterAbstract->m_fpImmunityFactors[l_tHitType]*l_fHit,_max(l_tpALifeMonsterAbstract->fHealth,0.f),l_tpALifeMonsterAbstract->fHealth >= EPS_L ? m_tpaCombatGroups[iCombatGroupIndex ^ 1].size() : m_tpaCombatGroups[iCombatGroupIndex ^ 1].size() - 1);
				}
#endif
				// check if victim became dead
				if (l_tpALifeMonsterAbstract->fHealth <= 0) {
					m_tpaCombatGroups[iCombatGroupIndex ^ 1].erase(m_tpaCombatGroups[iCombatGroupIndex ^ 1].begin() + l_iIndex);
					if (m_tpaCombatGroups[iCombatGroupIndex ^ 1].empty())
						return;
				}
			}
			else {
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg		("[LSS] %s missed",(*I)->s_name_replace);
				}
#endif
			}
			// perform attack (if we use a weapon we should delete ammo we used)
			if (!(*I)->bfPerformAttack())
				break;
		}
	}
}

void CSE_ALifeSimulator::vfAssignArtefactPosition(CSE_ALifeAnomalousZone *tpALifeAnomalousZone, CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	tpALifeDynamicObject->m_tGraphID		= tpALifeAnomalousZone->m_tGraphID;
	u32										l_dwIndex = tpALifeAnomalousZone->m_dwStartIndex + randI(tpALifeAnomalousZone->m_wArtefactSpawnCount);
	tpALifeDynamicObject->o_Position		= m_tpArtefactSpawnPositions[l_dwIndex].level_point();
	tpALifeDynamicObject->m_tNodeID			= m_tpArtefactSpawnPositions[l_dwIndex].level_vertex_id();
	tpALifeDynamicObject->m_fDistance		= m_tpArtefactSpawnPositions[l_dwIndex].distance();
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg									("[LSS] Zone %s[%f][%f][%f] %d: generated artefact position %s[%f][%f][%f]",tpALifeAnomalousZone->s_name_replace,VPUSH(tpALifeAnomalousZone->o_Position),tpALifeAnomalousZone->m_dwStartIndex,tpALifeDynamicObject->s_name_replace,VPUSH(tpALifeDynamicObject->o_Position));
	}
#endif
}

void CSE_ALifeSimulator::vfAssignDeathPosition(CSE_ALifeCreatureAbstract *tpALifeCreatureAbstract, _GRAPH_ID tGraphID, CSE_ALifeSchedulable *tpALifeSchedulable)
{
	tpALifeCreatureAbstract->fHealth		= 0;
	
	if (tpALifeSchedulable) {
		CSE_ALifeAnomalousZone				*l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable);
		if (l_tpALifeAnomalousZone) {
			vfAssignArtefactPosition		(l_tpALifeAnomalousZone,tpALifeCreatureAbstract);
			CSE_ALifeMonsterAbstract		*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeCreatureAbstract);
			if (l_tpALifeMonsterAbstract)
				l_tpALifeMonsterAbstract->m_tPrevGraphID = l_tpALifeMonsterAbstract->m_tNextGraphID = l_tpALifeMonsterAbstract->m_tGraphID;
			return;
		}
	}

	CGameGraph::const_spawn_iterator		i, e;
	ai().game_graph().begin_spawn			(tGraphID,i,e);
	i										+= (e != i) ? randI(s32(e - i)) : 0;
	tpALifeCreatureAbstract->m_tGraphID		= tGraphID;
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg									("[LSS] Generated death position %s[%f][%f][%f] -> [%f][%f][%f]",tpALifeCreatureAbstract->s_name_replace,VPUSH(tpALifeCreatureAbstract->o_Position),VPUSH((*i).level_point()));
	}
#endif
	tpALifeCreatureAbstract->o_Position		= (*i).level_point();
	tpALifeCreatureAbstract->m_tNodeID		= (*i).level_vertex_id();
	R_ASSERT2								((ai().game_graph().vertex(tGraphID)->level_id() != m_tCurrentLevelID) || ai().level_graph().valid_vertex_id(tpALifeCreatureAbstract->m_tNodeID),"Invalid vertex");
	tpALifeCreatureAbstract->m_fDistance	= (*i).distance();
	CSE_ALifeMonsterAbstract				*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeCreatureAbstract);
	if (l_tpALifeMonsterAbstract)
		l_tpALifeMonsterAbstract->m_tPrevGraphID = l_tpALifeMonsterAbstract->m_tNextGraphID = l_tpALifeMonsterAbstract->m_tGraphID;
}

void CSE_ALifeSimulator::vfAppendItemVector(OBJECT_VECTOR &tObjectVector, ITEM_P_VECTOR &tItemList)
{
	OBJECT_IT	I = tObjectVector.begin();
	OBJECT_IT	E = tObjectVector.end();
	for ( ; I != E; ++I) {
		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(object(*I));
		if (l_tpALifeInventoryItem)
			tItemList.push_back				(l_tpALifeInventoryItem);
	}
}

void CSE_ALifeSimulator::vfFinishCombat(ECombatResult tCombatResult)
{
	// processing weapons and dead monsters
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(m_tpaCombatObjects[0]);
	R_ASSERT2				(l_tpALifeDynamicObject,"Unknown schedulable object class");
	_GRAPH_ID				l_tGraphID = l_tpALifeDynamicObject->m_tGraphID;
	m_tpItemVector.clear();
	for (int i=0; i<2; ++i) {
		CSE_ALifeGroupAbstract	*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(m_tpaCombatObjects[i]);
		if (l_tpALifeGroupAbstract) {
			for (int I=0, N=l_tpALifeGroupAbstract->m_tpMembers.size() ; I<N; ++I) {
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(object(l_tpALifeGroupAbstract->m_tpMembers[I]));
				R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid group member!");
				l_tpALifeMonsterAbstract->vfUpdateWeaponAmmo	();
				if (l_tpALifeMonsterAbstract->fHealth <= EPS_L) {
					vfAppendItemVector							(l_tpALifeMonsterAbstract->children,m_tpItemVector);
					l_tpALifeMonsterAbstract->m_bDirectControl	= true;
					l_tpALifeGroupAbstract->m_tpMembers.erase	(l_tpALifeGroupAbstract->m_tpMembers.begin() + I);
					vfAssignDeathPosition						(l_tpALifeMonsterAbstract, l_tGraphID, m_tpaCombatObjects[i ^ 1]);
					l_tpALifeMonsterAbstract->vfDetachAll		();
					R_ASSERT									(l_tpALifeMonsterAbstract->children.empty());
					vfUpdateDynamicData							(l_tpALifeMonsterAbstract);
					CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeMonsterAbstract);
					if (l_tpALifeInventoryItem)
						m_tpItemVector.push_back				(l_tpALifeInventoryItem);
					--l_tpALifeGroupAbstract->m_wCount;
					--I;
					--N;
				}
			}
			if (!m_tpaCombatObjects[i]->bfActive())
				vfReleaseObject(m_tpaCombatObjects[i]);
		}
		else {
			m_tpaCombatObjects[i]->vfUpdateWeaponAmmo			();
			CSE_ALifeMonsterAbstract							*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(m_tpaCombatObjects[i]);
			if (l_tpALifeMonsterAbstract && (l_tpALifeMonsterAbstract->fHealth <= EPS_L)) {
				vfAppendItemVector								(l_tpALifeMonsterAbstract->children,m_tpItemVector);
				_GRAPH_ID										l_tGraphID1 = l_tpALifeMonsterAbstract->m_tGraphID;
				vfAssignDeathPosition							(l_tpALifeMonsterAbstract, l_tGraphID, m_tpaCombatObjects[i ^ 1]);
				l_tpALifeMonsterAbstract->vfDetachAll			();
				R_ASSERT										(l_tpALifeMonsterAbstract->children.empty());
				CSE_ALifeScheduleRegistry::remove				(l_tpALifeMonsterAbstract);
				if (l_tpALifeMonsterAbstract->m_tGraphID != l_tGraphID1) {
					vfRemoveObjectFromGraphPoint				(l_tpALifeMonsterAbstract,l_tGraphID1);
					vfAddObjectToGraphPoint						(l_tpALifeMonsterAbstract,l_tpALifeMonsterAbstract->m_tGraphID);
				}
				CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeMonsterAbstract);
				if (l_tpALifeInventoryItem)
					m_tpItemVector.push_back					(l_tpALifeInventoryItem);
			}
		}
	}
	
	if (m_tpItemVector.empty() || (eCombatTypeMonsterMonster != m_tCombatType)) {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] There is nothing to take");
		}
#endif
		return;
	}

	int			l_iGroupIndex = -1;
	switch (tCombatResult) {
		case eCombatResultBothKilled	:
		case eCombatResultRetreat12		: break;
		case eCombatResultRetreat2		:
		case eCombatResult1Kill2		: {
			l_iGroupIndex				= 0;
			break;
		}
		case eCombatResultRetreat1		:
		case eCombatResult2Kill1		: {
			l_iGroupIndex				= 1;
			break;
		}
		default							: NODEFAULT;
	}
	
	if (l_iGroupIndex >= 0) {
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Starting taking items [%s][%f]",m_tpaCombatObjects[l_iGroupIndex]->s_name_replace,dynamic_cast<CSE_ALifeMonsterAbstract*>(m_tpaCombatObjects[l_iGroupIndex])->fHealth);
		}
#endif
		m_tpaCombatObjects[l_iGroupIndex]->vfAttachItems();
	}
	m_tpItemVector.clear();
}
