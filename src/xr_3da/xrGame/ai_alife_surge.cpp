////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_surge.cpp
//	Created 	: 25.06.2003
//  Modified 	: 25.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation : Surge
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"
#include "graph_engine.h"
#include "ef_storage.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

CSE_Abstract *CSE_ALifeSimulator::spawn_item	(LPCSTR section, const Fvector &position, u32 level_vertex_id, _GRAPH_ID game_vertex_id, u16 parent_id)
{
	CSE_Abstract				*abstract = F_entity_Create(section);
	R_ASSERT3					(abstract,"Cannot find item with section %s",section);

	strcpy						(abstract->s_name,section);
	abstract->s_gameid			= u8(GameID());
	abstract->s_RP				= 0xff;
	abstract->ID				= m_tpServer->PerformIDgen(0xffff);
	abstract->ID_Parent			= parent_id;
	abstract->ID_Phantom		= 0xffff;
	abstract->o_Position		= position;
	
	strcpy						(abstract->s_name_replace,abstract->s_name);
	if (abstract->ID < 1000)
		strcat					(abstract->s_name_replace,"0");
	if (abstract->ID < 100)
		strcat					(abstract->s_name_replace,"0");
	if (abstract->ID < 10)
		strcat					(abstract->s_name_replace,"0");
	string16					S1;
	strcat						(abstract->s_name_replace,itoa(abstract->ID,S1,10));

	CSE_ALifeDynamicObject		*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(abstract);
	VERIFY						(dynamic_object);

	dynamic_object->m_tNodeID	= level_vertex_id;
	dynamic_object->m_tGraphID	= game_vertex_id;
	dynamic_object->m_tSpawnID	= u16(-1);

	CSE_ALifeObjectRegistry::Add(dynamic_object);

	vfUpdateDynamicData			(dynamic_object);

	return						(dynamic_object);
}

CSE_Abstract *CSE_ALifeSimulator::tpfCreateGroupMember(CSE_ALifeGroupAbstract *tpALifeGroupAbstract, CSE_ALifeDynamicObject *j)
{
	NET_Packet					tNetPacket;
	LPCSTR						S = pSettings->r_string(tpALifeGroupAbstract->s_name,"monster_section");
	CSE_Abstract				*l_tpAbstract = F_entity_Create(S);
	R_ASSERT2					(l_tpAbstract,"Can't create entity.");
	CSE_ALifeDynamicObject		*k = dynamic_cast<CSE_ALifeDynamicObject*>(l_tpAbstract);
	R_ASSERT2					(k,"Non-ALife object in the 'game.spawn'");

	j->Spawn_Write				(tNetPacket,TRUE);
	k->Spawn_Read				(tNetPacket);
	tNetPacket.w_begin			(M_UPDATE);
	j->UPDATE_Write				(tNetPacket);
	u16							id;
	tNetPacket.r_begin			(id);
	k->UPDATE_Read				(tNetPacket);
	strcpy						(k->s_name,S);
	k->m_tSpawnID				= j->m_tSpawnID;
	k->ID						= m_tpServer->PerformIDgen(0xffff);
	k->m_bDirectControl			= false;
	k->m_bALifeControl			= true;
	CSE_ALifeObjectRegistry::Add(k);
	strcpy						(k->s_name_replace,k->s_name);
	if (k->ID < 1000)
			strcat				(k->s_name_replace,"0");
	if (k->ID < 100)
		strcat					(k->s_name_replace,"0");
	if (k->ID < 10)
		strcat					(k->s_name_replace,"0");
	string16					S1;
	strcat						(k->s_name_replace,itoa(k->ID,S1,10));
	vfUpdateDynamicData			(k);
	k->spawn_supplies			();
	return						(k);
}

void CSE_ALifeSimulator::vfCreateObjectFromSpawnPoint(CSE_ALifeDynamicObject *&i, CSE_ALifeDynamicObject *j, _SPAWN_ID tSpawnID)
{
	CSE_Abstract				*tpSE_Abstract = F_entity_Create(j->s_name);
	R_ASSERT2					(tpSE_Abstract,"Can't create entity.");
	i							= dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
	R_ASSERT2					(i,"Non-ALife object in the 'game.spawn'");

	NET_Packet					tNetPacket;
	j->Spawn_Write				(tNetPacket,TRUE);
	i->Spawn_Read				(tNetPacket);
	tNetPacket.w_begin			(M_UPDATE);
	j->UPDATE_Write				(tNetPacket);
	u16							id;
	tNetPacket.r_begin			(id);
	i->UPDATE_Read				(tNetPacket);

	R_ASSERT3					(!(i->used_ai_locations()) || (i->m_tNodeID != u32(-1)),"Invalid vertex for object ",i->s_name_replace);

	i->m_tSpawnID				= tSpawnID;
	if (!m_tpActor && dynamic_cast<CSE_ALifeCreatureActor*>(i))
		i->ID					= 0;
	else
		i->ID					= m_tpServer->PerformIDgen(0xffff);
	CSE_ALifeObjectRegistry::Add(i);
	vfUpdateDynamicData			(i);
	i->m_bALifeControl			= true;

	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract	= dynamic_cast<CSE_ALifeMonsterAbstract*>(i);
	if (l_tpALifeMonsterAbstract)
		vfAssignGraphPosition	(l_tpALifeMonsterAbstract);

	CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(i);
	if (l_tpALifeGroupAbstract) {
		l_tpALifeGroupAbstract->m_tpMembers.resize(l_tpALifeGroupAbstract->m_wCount);
		OBJECT_IT				I = l_tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT				E = l_tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; ++I) {
			CSE_Abstract		*l_tpAbstract = tpfCreateGroupMember	(l_tpALifeGroupAbstract,j);
			*I					= l_tpAbstract->ID;
		}
	}
	else
		i->spawn_supplies		();
}

void CSE_ALifeSimulator::vfGenerateAnomalousZones()
{
	// deactivating all the anomalous zones
	D_OBJECT_PAIR_IT				B = m_tObjectRegistry.begin(), I = B, J;
	D_OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; ++I) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (l_tpALifeAnomalousZone)
			l_tpALifeAnomalousZone->m_maxPower = 0.f;
	}
	// for each spawn group activate a zone if any
	for (I = B; I != E; ) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!l_tpALifeAnomalousZone) {
			++I;
			continue;
		}

		// counting zones with the same group ID
		// !
		// spawn points are sorted according to their spawn group number in ascending order
		// this condition _must_ guarantee xrAI since it sorts spawn points during 'game.spawn' generation
		// we assume that zones in the spawn groups are generated in a row 
		// therefore they _must_ have their IDs in a row, since we starts their ID generation with a fullfilled
		// ID structure (look id_generator.h for details)
		// here we use this information by iterating on vector m_tpSpawnPoints and map m_tObjectRegistry 
		// (because this map is ordered by ID in ascending order)
		// if this condition is _not_ guaranteed we have to rewrite this piece of code
		// !
		float					fSum = 0;
		D_OBJECT_P_IT		i = m_tpSpawnPoints.begin() + l_tpALifeAnomalousZone->m_tSpawnID, j = i, e = m_tpSpawnPoints.end(), b = m_tpSpawnPoints.begin();
		u32						l_dwGroupID = (*i)->m_dwSpawnGroup;
		for ( ; j != e; ++j)
			if ((*j)->m_dwSpawnGroup != l_dwGroupID)
				break;
			else
				fSum += (*j)->m_fProbability;
		R_ASSERT2				(fSum < 1 + EPS_L,"Group probability more than 1!");

		// computing probability of the anomalous zone activation
		float					fProbability = randF(1.f);
		fSum					= 0.f;
		J						= I;
		D_OBJECT_P_IT		m = j;
		for ( j = i; (j != e) && ((*j)->m_dwSpawnGroup == l_dwGroupID); ++j, ++I) {
			fSum += (*j)->m_fProbability;
			if (fSum > fProbability)
				break;
		}

		// if random choosed a number due to which there is no active zones in the current group ID
		// then continue loop
		if (fSum <= fProbability)
			continue;

		// otherwise assign random anomaly power to the zone
		l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		R_ASSERT2				(l_tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
		CSE_ALifeAnomalousZone	*l_tpSpawnAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
		R_ASSERT2				(l_tpSpawnAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
#pragma todo("Dima to Dima : Correct anomalous zones power")
		l_tpALifeAnomalousZone->m_maxPower = l_tpALifeAnomalousZone->m_fStartPower = randF(50,150);

		// proceed random artefacts generation for the active zone
		for (u32 ii=0, jj=iFloor(l_tpALifeAnomalousZone->m_maxPower/20); ii<jj; ++ii) {
			fProbability		= randF(1.f);
			fSum				= 0;
			for (u16 p=0; p<l_tpSpawnAnomalousZone->m_wItemCount; ++p) {
				fSum			+= l_tpSpawnAnomalousZone->m_faWeights[p];
				if (fSum > fProbability)
					break;
			}
			if (p < l_tpSpawnAnomalousZone->m_wItemCount) {
				CSE_Abstract	*l_tpSE_Abstract = F_entity_Create(l_tpSpawnAnomalousZone->m_cppArtefactSections[p]);
				R_ASSERT2		(l_tpSE_Abstract,"Can't create entity.");
				CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
				R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");

				i->ID			= m_tpServer->PerformIDgen(0xffff);
				i->m_tSpawnID	= _SPAWN_ID(j - b);
				vfAssignArtefactPosition(l_tpSpawnAnomalousZone,i);
				i->m_bALifeControl = true;

				CSE_ALifeItemArtefact *l_tpALifeItemArtefact = dynamic_cast<CSE_ALifeItemArtefact*>(i);
				R_ASSERT2		(l_tpALifeItemArtefact,"Anomalous zone can't generate non-artefact objects since they don't have an 'anomaly property'!");

				l_tpALifeItemArtefact->m_fAnomalyValue = l_tpALifeAnomalousZone->m_maxPower*(1.f - i->o_Position.distance_to(l_tpSpawnAnomalousZone->o_Position)/l_tpSpawnAnomalousZone->m_fRadius);

				CSE_ALifeObjectRegistry::Add(i);
				strcpy					(l_tpALifeItemArtefact->s_name_replace,l_tpALifeItemArtefact->s_name);
				if (l_tpALifeItemArtefact->ID < 1000)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				if (l_tpALifeItemArtefact->ID < 100)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				if (l_tpALifeItemArtefact->ID < 10)
					strcat				(l_tpALifeItemArtefact->s_name_replace,"0");
				string16				S1;
				strcat					(l_tpALifeItemArtefact->s_name_replace,itoa(l_tpALifeItemArtefact->ID,S1,10));
				vfUpdateDynamicData		(i);
			}
		}
		++I;
	}
}

void CSE_ALifeSimulator::vfGenerateAnomalyMap()
{
	{
		ANOMALY_P_VECTOR_SIT	I = m_tpCrossAnomalies.begin();
		ANOMALY_P_VECTOR_SIT	E = m_tpCrossAnomalies.end();
		for ( ; I != E; ++I)
			(*I).clear();
	}

	m_tpAnomalies.resize		(ai().game_graph().header().vertex_count());
	D_OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; ++I) {
		CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>((*I).second);
		if (!l_tpALifeAnomalousZone || !randI(20))
			continue;

		CSE_ALifeKnownAnomaly	*l_tpALifeKnownAnomaly	= xr_new<CSE_ALifeKnownAnomaly>();
		l_tpALifeKnownAnomaly->m_tAnomalousZoneType		= randI(10) ? l_tpALifeAnomalousZone->m_tAnomalyType : EAnomalousZoneType(randI(eAnomalousZoneTypeDummy));
		l_tpALifeKnownAnomaly->m_fAnomalyPower			= randF(l_tpALifeAnomalousZone->m_maxPower*.5f,l_tpALifeAnomalousZone->m_maxPower*1.5f);
		l_tpALifeKnownAnomaly->m_fDistance				= randF(l_tpALifeAnomalousZone->m_fDistance*.5f,l_tpALifeAnomalousZone->m_fDistance*1.5f);
		l_tpALifeKnownAnomaly->m_tGraphID				= l_tpALifeAnomalousZone->m_tGraphID;
		m_tpAnomalies[l_tpALifeAnomalousZone->m_tGraphID].push_back(l_tpALifeKnownAnomaly);

		// updating known anomaly data
		m_tpCrossAnomalies[l_tpALifeKnownAnomaly->m_tAnomalousZoneType].push_back(l_tpALifeKnownAnomaly);
	}
}

void CSE_ALifeSimulator::vfBalanceCreatures()
{
#pragma todo("Dima to Dima : Respawn the objects in the spawn groups only")
	// filling array of the survived creatures
	{
		D_OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
		D_OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
		for ( ; I != E; ++I) {
			CSE_ALifeCreatureAbstract *l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>((*I).second);
			CSE_ALifeGroupAbstract	  *l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>((*I).second);
			if (l_tpALifeCreatureAbstract)
				if (l_tpALifeGroupAbstract) {
					if (l_tpALifeGroupAbstract->m_wCount) {
						m_baAliveSpawnObjects[(*I).second->m_tSpawnID] = true;
#pragma todo("Dima to Dima : Add monster population increase here")
						//l_tpALifeGroupAbstract->m_wCount *= l_tpALifeGroupAbstract->m_wCount < 50 ? 1.5 : 0.8;
					}
				}
				else
					if (l_tpALifeCreatureAbstract->fHealth > 0.f)
						m_baAliveSpawnObjects[(*I).second->m_tSpawnID] = true;
		}
	}
	// balancing creatures by spawn groups
	// i.e. if there is no object being spawned by the particular spawn group
	// then we have to spawn an object from this spawn group
	{
		D_OBJECT_P_IT			B = m_tpSpawnPoints.begin(), I = B, J;
		D_OBJECT_P_IT			E = m_tpSpawnPoints.end();
		for ( ; I != E; ) {
			u32						l_dwSpawnGroup = (*I)->m_dwSpawnGroup;
			bool					bOk = false;
			J						= I;
			for ( ; (I != E) && (l_dwSpawnGroup == (*I)->m_dwSpawnGroup); ++I)
				if (m_baAliveSpawnObjects[I - B]) {
					bOk = true;
					++I;
					break;
				}
			if (!bOk) {
				// there is no object being spawned from this spawn group -> spawn it!
				float				l_fProbability = randF(0,1.f), l_fSum = 0.f;
				D_OBJECT_P_IT	j = J;
				D_OBJECT_P_IT	e = I;
				for ( ; (j != e); ++j) {
					l_fSum			+= (*j)->m_fProbability;
					if (l_fSum > l_fProbability)
						break;
				}
				if (l_fSum > l_fProbability) {
					CSE_ALifeAnomalousZone		*l_tpALifeAnomalousZone		= dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
					if (l_tpALifeAnomalousZone)
						continue;

#pragma todo("Dima to Dima : Decide where to spawn an actor!")
//					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract	= dynamic_cast<CSE_ALifeCreatureAbstract*>(*j);
//					if (l_tpALifeCreatureAbstract)
//						continue;

					CSE_ALifeDynamicObject		*l_tpALifeDynamicObject;
					vfCreateObjectFromSpawnPoint(l_tpALifeDynamicObject,*j,_SPAWN_ID(j - B));
				}
			}
		}
	}

	// initialize array
	m_baAliveSpawnObjects.assign(m_baAliveSpawnObjects.size(),false);
}

void CSE_ALifeSimulator::vfKillCreatures()
{
	D_OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
	for ( ; I != E; ++I) {
		CSE_ALifeCreatureAbstract *l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>((*I).second);
		if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->m_bDirectControl) && (l_tpALifeCreatureAbstract->fHealth > 0.f)) {
			CSE_ALifeGroupAbstract *l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>((*I).second);
			ai().ef_storage().m_tpCurrentALifeObject = (*I).second;
			if (l_tpALifeGroupAbstract) {
				_GRAPH_ID			l_tGraphID = l_tpALifeCreatureAbstract->m_tGraphID;
				for (u32 i=0, N = (u32)l_tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>(object(l_tpALifeGroupAbstract->m_tpMembers[i]));
					R_ASSERT2					(l_tpALifeCreatureAbstract,"Group class differs from the member class!");
					ai().ef_storage().m_tpCurrentALifeObject = l_tpALifeCreatureAbstract;
					if (randF(100) > ai().ef_storage().m_pfSurgeDeathProbability->ffGetValue()) {
						l_tpALifeCreatureAbstract->m_bDirectControl	= true;
						l_tpALifeGroupAbstract->m_tpMembers.erase	(l_tpALifeGroupAbstract->m_tpMembers.begin() + i);
						vfUpdateDynamicData							(l_tpALifeCreatureAbstract);
						vfAssignDeathPosition						(l_tpALifeCreatureAbstract,l_tGraphID);
						--i;
						--N;
					}
				}
			}
			else
				if (randI(100) < ai().ef_storage().m_pfSurgeDeathProbability->ffGetValue())
					l_tpALifeCreatureAbstract->fHealth = 0.f;
		}
	}
}

void CSE_ALifeSimulator::vfGiveMilitariesBribe(CSE_ALifeTrader &tTrader)
{
	tTrader.m_dwMoney = u32(float(tTrader.m_dwMoney)*.9f);
}

void CSE_ALifeSimulator::vfUpdateOrganizations()
{
	// iterating on all the organizations
	ORGANIZATION_P_PAIR_IT	I = m_tOrganizationRegistry.begin();
	ORGANIZATION_P_PAIR_IT	E = m_tOrganizationRegistry.end();
	for ( ; I != E; ++I) {
		switch ((*I).second->m_tResearchState) {
			case eResearchStateLeft : {
				// asking if we have to join zone investigations
				if (randF(1) >= (*I).second->m_fJoinProbability)
					break;

				(*I).second->m_tResearchState = eResearchStateJoin;
			}
			case eResearchStateJoin : {
				// asking if we have to left zone investigations
				if (!xr_strlen((*I).second->m_caDiscoveryToInvestigate) && (*I).second->m_tpOrderedArtefacts.empty() && (randF(1) < (*I).second->m_fLeftProbability))
					(*I).second->m_tResearchState = eResearchStateLeft;
				else {
					// selecting discovery we would like to investigate
					CSE_ALifeDiscovery	*l_tpBestDiscovery = 0;
					bool				l_bGoToResearch = false;
					LPSTR_IT			i = (*I).second->m_tpPossibleDiscoveries.begin();
					LPSTR_IT			e = (*I).second->m_tpPossibleDiscoveries.end();
					for ( ; i != e; ++i) {
						// getting pointer to discovery object from the discovery registry
						DISCOVERY_P_PAIR_IT			j = m_tDiscoveryRegistry.find(*i);
						R_ASSERT2					(m_tDiscoveryRegistry.end() != j,"Invalid discovery name in the possible iscoveries parameters in the 'system.ltx'!");

						// checking if discovery has not been invented yet
						if ((*j).second->m_bAlreadyInvented)
							continue;

						// checking if discovery depends on the non-invented discoveries
						LPSTR_IT					II = (*j).second->m_tpDependency.begin();
						LPSTR_IT					EE = (*j).second->m_tpDependency.end();
						bool						l_bFoundDiscovery = true;
						for ( ; II != EE; ++II) {
							DISCOVERY_P_PAIR_IT		J = m_tDiscoveryRegistry.find(*II);
							R_ASSERT2				(m_tDiscoveryRegistry.end() != J,"Invalid discovery dependency!");
							if (!(*J).second->m_bAlreadyInvented) {
								l_bFoundDiscovery = false;
								break;
							}
						}
						if (!l_bFoundDiscovery)
							break;

						// checking if artefacts being used during discovery invention are known
						// and there are enough artefacts purchased for discovery
						ARTEFACT_ORDER_IT			ii = (*j).second->m_tpArtefactNeed.begin();
						ARTEFACT_ORDER_IT			ee = (*j).second->m_tpArtefactNeed.end();
						bool						l_bIsReadyForInvention = true;
						for ( ; ii != ee; ++ii) {
							LPSTR_BOOL_PAIR_IT		iii = m_tArtefactRegistry.find((*ii).m_caSection);
							R_ASSERT3				(m_tArtefactRegistry.end() != iii,"Unknown artefact",(*ii).m_caSection);
							if ((*iii).second) {
								l_bFoundDiscovery	= false;
								break;
							}
							else {
								ITEM_COUNT_PAIR_IT	jj = (*I).second->m_tpPurchasedArtefacts.find((LPSTR)(*ii).m_caSection);
								if (((*I).second->m_tpPurchasedArtefacts.end() == jj) || ((*jj).second < (*ii).m_dwCount))
									l_bIsReadyForInvention = false;
							}
						}
						if (l_bFoundDiscovery) {
							l_tpBestDiscovery		= (*j).second;
							l_bGoToResearch			= l_bIsReadyForInvention;
						}
					}
					// checking if we did select the discovery
					if (l_tpBestDiscovery) {
						strcpy((*I).second->m_caDiscoveryToInvestigate,  l_tpBestDiscovery->m_caDiscoveryIdentifier);
						// checking if we are ready to invent it
						if (l_bGoToResearch) {
							(*I).second->m_tResearchState = eResearchStateResearch;
							(*I).second->m_tpPurchasedArtefacts.clear();
						}
						else {
							// if not - order artefacts needed for the discovery
							(*I).second->m_tpOrderedArtefacts = l_tpBestDiscovery->m_tpArtefactNeed;
						}
					}
					else
						(*I).second->m_tpPurchasedArtefacts.clear();
				}
				break;
			}
			case eResearchStateResearch : {
				// getting pointer to discovery object from the discovery registry
				DISCOVERY_P_PAIR_IT	i = m_tDiscoveryRegistry.find((*I).second->m_caDiscoveryToInvestigate);
				R_ASSERT2(m_tDiscoveryRegistry.end() != i,"Unknown discovery!");

				// checking if we've got any result
				if (randF(1) < (*i).second->m_fResultProbability) {
					// checking if we've invented the discovery
					if (randF(1) < (*i).second->m_fSuccessProbability) {
						R_ASSERT2(!(*i).second->m_bAlreadyInvented,"Discovery has been already invented!");
						(*i).second->m_bAlreadyInvented = true;
						(*I).second->m_tResearchState = eResearchStateJoin;
						// ordering artefacts
						DEMAND_P_IT			ii = (*i).second->m_tpArtefactDemand.begin();
						DEMAND_P_IT			ee = (*i).second->m_tpArtefactDemand.end();
						(*I).second->m_tpOrderedArtefacts.resize(ee - ii);
						ARTEFACT_ORDER_IT		II = (*I).second->m_tpOrderedArtefacts.begin();
						for ( ; ii != ee; ++ii, ++II) {
							strcpy((*II).m_caSection,(*ii)->m_caSection);
							(*II).m_dwCount	= randI((*ii)->m_dwMinArtefactCount,(*ii)->m_dwMaxArtefactCount);
							(*II).m_dwPrice	= randI((*ii)->m_dwMinArtefactPrice,(*ii)->m_dwMaxArtefactPrice);
						}
					}
					else
						// checking if we've destroyed laboratory during investigations
						if (randF(1) < (*i).second->m_fDestroyProbability) {
							(*I).second->m_tResearchState = eResearchStateFreeze;
						}
						else {
							// otherwise - we finished with investigations without any discovery;
							(*I).second->m_tResearchState = eResearchStateJoin;
						}
				}
				break;
			}
			case eResearchStateFreeze : {
				// getting pointer to discovery object from the discovery registry
				DISCOVERY_P_PAIR_IT	i = m_tDiscoveryRegistry.find((*I).second->m_caDiscoveryToInvestigate);
				R_ASSERT2(m_tDiscoveryRegistry.end() != i,"Unknown discovery!");
				// checking if we've restored
				if (randF(1) < (*i).second->m_fUnfreezeProbability)
					(*I).second->m_tResearchState = eResearchStateJoin;
				break;
			}
			default : NODEFAULT;
		}
	}
}

void CSE_ALifeSimulator::vfSellArtefacts(CSE_ALifeTrader &tTrader)
{
	// filling temporary map with the purchased artefacts
	m_tpTraderItems.clear();
	{
		xr_vector<u16>::iterator	i = tTrader.children.begin();
		xr_vector<u16>::iterator	e = tTrader.children.end();
		for ( ; i != e; ++i) {
			// checking if the purchased item is an artefact
			CSE_ALifeItemArtefact *l_tpALifeItemArtefact = dynamic_cast<CSE_ALifeItemArtefact*>(object(*i));
			if (!l_tpALifeItemArtefact)
				continue;

			// adding item to the temporary artefact map
			ITEM_COUNT_PAIR_IT		k = m_tpTraderItems.find(l_tpALifeItemArtefact->s_name);
			if (m_tpTraderItems.end() == k)
				m_tpTraderItems.insert(mk_pair(l_tpALifeItemArtefact->s_name,1));
			else
				++((*k).second);
		}
	}

	// iterating on all the trader artefacts
	ITEM_COUNT_PAIR_IT				i = m_tpTraderItems.begin();
	ITEM_COUNT_PAIR_IT				e = m_tpTraderItems.end();
	for ( ; i != e; ++i) {
		m_tpSoldArtefacts.clear	();
		// iterating on all the organizations
		{
			ORGANIZATION_P_PAIR_IT	I = m_tOrganizationRegistry.begin();
			ORGANIZATION_P_PAIR_IT	E = m_tOrganizationRegistry.end();
			for ( ; I != E; ++I) {
				// checking if our rank is enough for the organization
				// and the organization in the appropriate state
				if (((*I).second->m_tOrgID == tTrader.m_tOrgID) && (eResearchStateJoin == (*I).second->m_tResearchState)) {
					// iterating on all the organization artefact orders
					ARTEFACT_ORDER_IT		ii = (*I).second->m_tpOrderedArtefacts.begin();
					ARTEFACT_ORDER_IT		ee = (*I).second->m_tpOrderedArtefacts.end();
					for ( ; ii != ee; ++ii)
						if (!xr_strcmp((*ii).m_caSection,(*i).first)) {
							SOrganizationOrder							l_tOrganizationOrder;
							l_tOrganizationOrder.m_tpALifeOrganization	= (*I).second;
							l_tOrganizationOrder.m_dwCount				= (*ii).m_dwCount;
							m_tpSoldArtefacts.insert					(mk_pair((*ii).m_dwPrice,l_tOrganizationOrder));
							break;
						}
				}
			}
		}
		// iterating on all the orders in descending order in order to get the maximum profit
		{
			ORGANIZATION_ORDER_PAIR_IT	I = m_tpSoldArtefacts.begin();
			ORGANIZATION_ORDER_PAIR_IT	E = m_tpSoldArtefacts.end();
			for ( ; I != E; ++I) {
				// checking if organization has already bought these artefacts
				ITEM_COUNT_PAIR_IT		j = (*I).second.m_tpALifeOrganization->m_tpPurchasedArtefacts.find((*i).first);
				if ((*I).second.m_tpALifeOrganization->m_tpPurchasedArtefacts.end() == j)
					(*I).second.m_tpALifeOrganization->m_tpPurchasedArtefacts.insert(mk_pair((*i).first,(*I).second.m_dwCount));
				else
					(*j).second			+= (*I).second.m_dwCount;
				// adding money to trader
				tTrader.m_dwMoney		+= (*I).first*(*I).second.m_dwCount;
				// removing sold objects from trader ownership
				{
					bool						l_bFoundObject = false;
					xr_vector<u16>::iterator	ii = tTrader.children.begin();
					xr_vector<u16>::iterator	ee = tTrader.children.end();
					for ( ; ii != ee; ++ii) {
						// getting pointer to the purchased item from the object registry
						CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = object(*ii);
						if (!xr_strcmp((*i).first,l_tpALifeDynamicObject->s_name)) {
							CSE_Abstract				*l_tpAbstract = dynamic_cast<CSE_Abstract*>(l_tpALifeDynamicObject);
							m_tpServer->entity_Destroy	(l_tpAbstract);
							xr_delete					(l_tpAbstract); 
							m_tObjectRegistry.erase		(*ii);
							tTrader.children.erase		(ii);
							l_bFoundObject				= true;
							break;
						}
					}
					R_ASSERT2					(l_bFoundObject,"Not enough objects to sell!");
				}
			}
		}
	}
}

void CSE_ALifeSimulator::vfUpdateArtefactOrders(CSE_ALifeTrader &tTrader)
{
	// iterating on all the organizations
	ORGANIZATION_P_PAIR_IT	I = m_tOrganizationRegistry.begin();
	ORGANIZATION_P_PAIR_IT	E = m_tOrganizationRegistry.end();
	for ( ; I != E; ++I) {
		// checking if our rank is enough for the organization
		// and the organization in the appropriate state
		if (((*I).second->m_tOrgID == tTrader.m_tOrgID) && (eResearchStateJoin == (*I).second->m_tResearchState)) {
			ARTEFACT_ORDER_IT		i = (*I).second->m_tpOrderedArtefacts.begin();
			ARTEFACT_ORDER_IT		e = (*I).second->m_tpOrderedArtefacts.end();
			for ( ; i != e; ++i) {
				ARTEFACT_TRADER_ORDER_PAIR_IT	ii = tTrader.m_tpOrderedArtefacts.find((*i).m_caSection);
				if (tTrader.m_tpOrderedArtefacts.end() != ii) {
					(*ii).second->m_dwTotalCount += (*i).m_dwCount;
					SArtefactOrder	l_tArtefactOrder;
					l_tArtefactOrder.m_dwCount	= (*i).m_dwCount;
					l_tArtefactOrder.m_dwPrice	= (*i).m_dwPrice;
					strcpy						(l_tArtefactOrder.m_caSection,(*I).first);
					(*ii).second->m_tpOrders.push_back(l_tArtefactOrder);
				}
				else {
					SArtefactTraderOrder	*l_tpTraderArtefactOrder = xr_new<SArtefactTraderOrder>();
					strcpy					(l_tpTraderArtefactOrder->m_caSection,(*i).m_caSection);
					l_tpTraderArtefactOrder->m_dwTotalCount = (*i).m_dwCount;
					SArtefactOrder			l_tArtefactOrder;
					l_tArtefactOrder.m_dwCount	= (*i).m_dwCount;
					l_tArtefactOrder.m_dwPrice	= (*i).m_dwPrice;
					strcpy						(l_tArtefactOrder.m_caSection,(*I).first);
					l_tpTraderArtefactOrder->m_tpOrders.push_back(l_tArtefactOrder);
					tTrader.m_tpOrderedArtefacts.insert(mk_pair(l_tpTraderArtefactOrder->m_caSection,l_tpTraderArtefactOrder));
					R_ASSERT				(tTrader.m_tpOrderedArtefacts.find(l_tpTraderArtefactOrder->m_caSection) != tTrader.m_tpOrderedArtefacts.end());
					// updating cross traders table
					TRADER_SET_PAIR_IT	J = m_tpCrossTraders.find((*i).m_caSection);
					if (m_tpCrossTraders.end() == J) {
						m_tpCrossTraders.insert(mk_pair((*i).m_caSection,TRADER_SET()));
						J = m_tpCrossTraders.find((*i).m_caSection);
						R_ASSERT2	(m_tpCrossTraders.end() != J,"Cannot append the cross trader map!");
					}
					TRADER_SET_IT	K = (*J).second.find(&tTrader);
					if ((*J).second.end() == K)
						(*J).second.insert(&tTrader);
				}
			}
		}
	}
	{
		ARTEFACT_TRADER_ORDER_PAIR_IT	I = tTrader.m_tpOrderedArtefacts.begin();
		ARTEFACT_TRADER_ORDER_PAIR_IT	E = tTrader.m_tpOrderedArtefacts.end();
		for ( ; I != E; ++I)
			std::sort				((*I).second->m_tpOrders.begin(),(*I).second->m_tpOrders.end(),CArtefactOrderPredicate());
	}
}

void CSE_ALifeSimulator::vfBuySupplies(CSE_ALifeTrader &tTrader)
{
	// filling temporary map with the purchased items
	m_tpTraderItems.clear();
	{
		xr_vector<u16>::iterator	i = tTrader.children.begin();
		xr_vector<u16>::iterator	e = tTrader.children.end();
		for ( ; i != e; ++i) {
			// checking if the purchased item is an item
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(object(*i));
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory object has a parent?!");
			// adding item to the temporary item map
			ITEM_COUNT_PAIR_IT		k = m_tpTraderItems.find(l_tpALifeInventoryItem->s_name);
			if (m_tpTraderItems.end() == k)
				m_tpTraderItems.insert(mk_pair(l_tpALifeInventoryItem->s_name,1));
			else
				++((*k).second);
		}
	}

	// iterating on all the supply orders
	string64						S, S1;
	TRADER_SUPPLY_IT				I = tTrader.m_tpSupplies.begin();
	TRADER_SUPPLY_IT				E = tTrader.m_tpSupplies.end();
	for ( ; I != E; ++I) {
		// choosing item to purchase
		u32							l_dwIndex = randI(_GetItemCount((*I).m_caSections));
		_GetItem					((*I).m_caSections,l_dwIndex,S);
		
		// checking if item dependent discoveries are invented
		LPCSTR						l_caItemDependencies = pSettings->r_string(S,"discovery_dependency");
		bool						l_bIndependent = true;
		for (u32 i=0, n = _GetItemCount(l_caItemDependencies); i<n; ++i)
			if (m_tDiscoveryRegistry.end() == m_tDiscoveryRegistry.find(_GetItem(S,i,S1))) {
				l_bIndependent = false;
				break;
			}
		if (!l_bIndependent)
			continue;
		
		// counting how much items of the "item" we have to purchase
		ITEM_COUNT_PAIR_IT			j = m_tpTraderItems.find(S);
		u32							l_dwItemCount = 0;
		if (j != m_tpTraderItems.end())
			l_dwItemCount			= (*j).second;
		int							l_iDifference = int((*I).m_dwCount) - int(l_dwItemCount);
		if (l_iDifference > 0) {
			l_iDifference			= iFloor(float(l_iDifference)*randF((*I).m_fMinFactor,(*I).m_fMaxFactor) + .5f);
			// purchase an item
			for (int p=0; p<l_iDifference; ++p) {
				// create item object
				CSE_Abstract	*l_tpSE_Abstract = F_entity_Create	(S);
				R_ASSERT2		(l_tpSE_Abstract,"Can't create entity.");
				CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
				R_ASSERT2		(i,"Non-ALife object in the 'game.spawn'");
				CSE_ALifeItem	*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(i);
				R_ASSERT2		(l_tpALifeItem,"Non-item object in the trader supplies string!");
				
				// checking if there is enough money to buy an item
				if (l_tpALifeItem->m_dwCost > tTrader.m_dwMoney) {
					xr_delete	(l_tpSE_Abstract);
					break;
				}
				tTrader.m_dwMoney			-= l_tpALifeItem->m_dwCost;
				l_tpALifeItem->ID			= m_tpServer->PerformIDgen(0xffff);
				l_tpALifeItem->ID_Parent	= tTrader.ID;
				l_tpALifeItem->m_tSpawnID	= _SPAWN_ID(-1);
				l_tpALifeItem->m_tGraphID	= tTrader.m_tGraphID;
				l_tpALifeItem->o_Position	= tTrader.o_Position;
				l_tpALifeItem->m_tNodeID	= tTrader.m_tNodeID;
				l_tpALifeItem->m_fDistance	= tTrader.m_fDistance;
				l_tpALifeItem->m_bALifeControl = true;
				CSE_ALifeObjectRegistry::Add(i);
				vfUpdateDynamicData			(i);
			}
		}
	}
}

float CSE_ALifeSimulator::distance(const DWORD_VECTOR &path) const
{
	float							distance = 0.f;
	xr_vector<u32>::const_iterator	I = path.begin();
	xr_vector<u32>::const_iterator	E = path.end() - 1;
	for ( ; I != E; ++I) {
		CGameGraph::const_iterator	i, e;
		bool						ok = false;
		ai().game_graph().begin		(*I,i,e);
		for ( ; i != e; ++i)
			if ((*i).vertex_id() == *(I + 1)) {
				distance			+= (*i).distance();
				ok					= true;
				break;
			}
		VERIFY						(ok);
	}
	return							(distance);
}

void CSE_ALifeSimulator::vfUpdateTasks()
{
	m_tTaskRegistry.clear		();
	m_tTaskID					= 0;
	
	// iterating on the ordered artefacts (this map is constructed from all the traders orders)
	TRADER_SET_PAIR_IT			I = m_tpCrossTraders.begin();
	TRADER_SET_PAIR_IT			E = m_tpCrossTraders.end();
	for ( ; I != E; ++I) {
		ITEM_SET_PAIR_IT		J = m_tArtefactAnomalyMap.find((*I).first);
		if (m_tArtefactAnomalyMap.end() == J)
			continue;
		// iterating on the anomaly types that can generate this type of artefact
		U32_SET_IT				i = (*J).second.begin();
		U32_SET_IT				e = (*J).second.end();
		for ( ; i != e; ++i) {
			// iterating on all the active anomalies by the particular type
			ANOMALY_P_IT		II = m_tpCrossAnomalies[*i].begin();
			ANOMALY_P_IT		EE = m_tpCrossAnomalies[*i].end();
			for ( ; II != EE; ++II) {
				// iterating on all the traders who ordered this type of artefact
				float			l_fMinDistance = 10000000.f;
				TRADER_SET_IT	ii = (*I).second.begin();
				TRADER_SET_IT	ee = (*I).second.end(), jj = ee;
				for ( ; ii != ee; ++ii) {
					bool		successful = ai().graph_engine().search(ai().game_graph(),(*ii)->m_tGraphID,(*II)->m_tGraphID,&m_tpTempPath,CGraphEngine::CBaseParameters());
					if (!successful)
						continue;
					float		l_fDistance = distance(m_tpTempPath);
					if (l_fDistance < l_fMinDistance) {
						l_fMinDistance = l_fDistance;
						jj = ii;
					}
				}
				R_ASSERT2		(jj != ee,"There is no way from trader to artefact");

				// creating _new task
				CSE_ALifeTask				*l_tpALifeTask = xr_new<CSE_ALifeTask>();
				l_tpALifeTask->m_tTaskType	= eTaskTypeSearchForItemCG;
				strcpy						(l_tpALifeTask->m_caSection,(*I).first);
				l_tpALifeTask->m_tGraphID	= (*II)->m_tGraphID;
				l_tpALifeTask->m_tTimeID	= tfGetGameTime();
				l_tpALifeTask->m_tCustomerID= (*jj)->ID;
#pragma todo("Dima to Dima : Correct task price")
				l_tpALifeTask->m_fCost		= 100.f;
				l_tpALifeTask->m_tTaskID	= m_tTaskID++;
				CSE_ALifeTaskRegistry::Add	(l_tpALifeTask);
			}
		}
	}
}

void CSE_ALifeSimulator::vfAssignStalkerCustomers()
{
	SCHEDULE_P_PAIR_IT				I = m_tpScheduledObjects.begin();
	SCHEDULE_P_PAIR_IT				E = m_tpScheduledObjects.end();
	for ( ; I != E; ++I) {
		CSE_ALifeHumanAbstract		*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>((*I).second);
		if (l_tpALifeHumanAbstract && xr_strlen(l_tpALifeHumanAbstract->m_caKnownCustomers)) {
//			u32						N = _GetItemCount(l_tpALifeHumanAbstract->m_caKnownCustomers);
//			if (!N)
//				continue;
//			
//			string32				S;
//			l_tpALifeHumanAbstract->m_tpKnownCustomers.clear();
//			for (u32 i=0; i<N; ++i) {
//				_GetItem(l_tpALifeHumanAbstract->m_caKnownCustomers,i,S);
//				bool				bOk = false;
//				D_OBJECT_PAIR_IT		II = m_tObjectRegistry.begin();
//				D_OBJECT_PAIR_IT		EE = m_tObjectRegistry.end();
//				for ( ; II != EE; ++II) {
//					CSE_ALifeTraderAbstract *l_tpTraderAbstract = dynamic_cast<CSE_ALifeTraderAbstract*>((*II).second);
//					if (l_tpTraderAbstract) {
//						if (!xr_strcmp((*II).second->s_name_replace,S)) {
//							l_tpALifeHumanAbstract->m_tpKnownCustomers.push_back((*II).second->ID);
//							bOk		= true;
//							break;
//						}
//					}
//				}
//				R_ASSERT3			(bOk,"There is no customer for the stalker ",l_tpALifeHumanAbstract->s_name_replace);
//			}
			l_tpALifeHumanAbstract->m_tpKnownCustomers.push_back(trader_nearest(l_tpALifeHumanAbstract)->ID);
			l_tpALifeHumanAbstract->m_caKnownCustomers = 0;
		}
	}
}

void CSE_ALifeSimulator::vfPerformSurge()
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Surge started");
	}
#endif
	seed							(s32(CPU::GetCycleCount() & 0xffffffff));
	vfGenerateAnomalousZones		();
	vfGenerateAnomalyMap			();
	vfKillCreatures					();
	vfBalanceCreatures				();
	{
		TRADER_P_IT					I = m_tpTraders.begin();
		TRADER_P_IT					E = m_tpTraders.end();
		for ( ; I != E; ++I) {
			vfSellArtefacts			(**I);
			vfBuySupplies			(**I);
			vfGiveMilitariesBribe	(**I);
		}
	}
	vfUpdateOrganizations			();
	{
		TRADER_P_IT					I = m_tpTraders.begin();
		TRADER_P_IT					E = m_tpTraders.end();
		for ( ; I != E; ++I)
			vfUpdateArtefactOrders	(**I);
	}
	vfUpdateTasks					();
	vfAssignStalkerCustomers		();
	
	// updating dynamic data
	vfUpdateDynamicData				(false);
}