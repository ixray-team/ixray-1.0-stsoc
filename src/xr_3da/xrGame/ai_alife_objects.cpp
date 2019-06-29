////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.cpp
//	Created 	: 23.06.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_objects.h"
#include "ai_alife_registries.h"
#include "ai_alife_templates.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "../xrLevel.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeSpawnHeader
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeSpawnHeader::Load	(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(SPAWN_POINT_CHUNK_VERSION),"Can't find chunk SPAWN_POINT_CHUNK_VERSION!");
	m_tSpawnVersion				= tFileStream.r_u32();
	R_ASSERT2					(XRAI_CURRENT_VERSION == m_tSpawnVersion,"'game.spawn' version mismatch!");
	m_dwSpawnCount				= tFileStream.r_u32();
	m_dwLevelCount				= tFileStream.r_u32();
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeDiscovery
////////////////////////////////////////////////////////////////////////////
CSE_ALifeDiscovery::CSE_ALifeDiscovery()
{
	R_ASSERT2					(false,"Unexpcted default constructor call!");
}

CSE_ALifeDiscovery::CSE_ALifeDiscovery(LPCSTR caSection)
{
	m_caDiscoveryIdentifier		= pSettings->r_string	(caSection,"name");
	m_fSuccessProbability		= pSettings->r_float	(caSection,"success_probability");
	m_fDestroyProbability		= pSettings->r_float	(caSection,"destroy_probability");
	m_fUnfreezeProbability		= pSettings->r_float	(caSection,"unfreeze_probability");
	m_fResultProbability		= pSettings->r_float	(caSection,"result_probability");
	m_bAlreadyInvented			= !!pSettings->r_bool(caSection,"already_invented");

	LPCSTR						S;
	string64					S1;
	{
		S						= pSettings->r_string	(caSection,"demand_on_success");
		R_ASSERT2				(!(_GetItemCount(S) % 5),"Invalid argument count in the discovery object section!");
		m_tpArtefactDemand.resize(_GetItemCount(S)/5);
		DEMAND_P_IT				B = m_tpArtefactDemand.begin(), I = B;
		DEMAND_P_IT				E = m_tpArtefactDemand.end();
		for ( ; I != E; ++I)
			*I = xr_new<CSE_ALifeArtefactDemand>(_GetItem(S,5*int(I - B) + 0,S1),atoi(_GetItem(S,5*int(I - B) + 1,S1)),atoi(_GetItem(S,5*int(I - B) + 2,S1)),atoi(_GetItem(S,5*int(I - B) + 3,S1)),atoi(_GetItem(S,5*int(I - B) + 4,S1)));
	}
	{
		S						= pSettings->r_string	(caSection,"artefacts");
		R_ASSERT2				(!(_GetItemCount(S) % 3),"Invalid argument count in the discovery object section!");
		u32						l_dwCount = _GetItemCount(S)/3;
		for (u32 i=0; i<l_dwCount; ++i) {
			SArtefactOrder		l_tArtefactOrder;
			_GetItem			(S,3*i + 0,l_tArtefactOrder.m_caSection);
			l_tArtefactOrder.m_dwCount = atoi(_GetItem(S,3*i + 1,S1));
			l_tArtefactOrder.m_dwPrice = atoi(_GetItem(S,3*i + 2,S1));
			m_tpArtefactNeed.push_back(l_tArtefactOrder);
		}
	}
	{
		S						= pSettings->r_string	(caSection,"discovery_dependence");
		m_tpDependency.resize	(_GetItemCount(S));
		LPSTR_IT				B = m_tpDependency.begin(), I = B;
		LPSTR_IT				E = m_tpDependency.end();
		for ( ; I != E; ++I) {
			_GetItem			(S,int(I - B),S1);
			*I					= (char*)xr_malloc((xr_strlen(S1) + 1)*sizeof(char));
			strcpy				((char*)(*I),S1);
		}
	}
}

CSE_ALifeDiscovery::~CSE_ALifeDiscovery()
{
	delete_data					(m_tpArtefactDemand);
	delete_data					(m_tpDependency);
}

void CSE_ALifeDiscovery::Save	(IWriter &tMemoryStream)
{
	tMemoryStream.w_u32			(u32(m_bAlreadyInvented));
}

void CSE_ALifeDiscovery::Load	(IReader &tFileStream)
{
	m_bAlreadyInvented			= !!tFileStream.r_u32();
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeOrganization
////////////////////////////////////////////////////////////////////////////
CSE_ALifeOrganization::CSE_ALifeOrganization()
{
	R_ASSERT2					(false,"Unexpcted default constructor call!");
}

CSE_ALifeOrganization::CSE_ALifeOrganization(LPCSTR caSection)
{
	LPCSTR						S;
	string64					S1;
	S							= pSettings->r_string	(caSection,"discoveries");
	m_tpPossibleDiscoveries.resize(_GetItemCount(S));
	LPSTR_IT					B = m_tpPossibleDiscoveries.begin(), I = B;
	LPSTR_IT					E = m_tpPossibleDiscoveries.end();
	for ( ; I != E; ++I) {
		_GetItem				(S,int(I - B),S1);
		*I						= (char*)xr_malloc((xr_strlen(S1) + 1)*sizeof(char));
		strcpy					((char*)(*I),S1);
	}
	m_fJoinProbability			= pSettings->r_float	(caSection,"join_probability");
	m_fLeftProbability			= pSettings->r_float	(caSection,"left_probability");
	m_tOrgID					= _ORGANIZATION_ID		(pSettings->r_u32(caSection,"trader_rank"));
	strcpy						(m_caDiscoveryToInvestigate,"");
	m_tResearchState			= eResearchStateLeft;
}

CSE_ALifeOrganization::~CSE_ALifeOrganization()
{
	delete_data					(m_tpPossibleDiscoveries);
	delete_data					(m_tpPurchasedArtefacts);
}

void CSE_ALifeOrganization::Save(IWriter &tMemoryStream)
{
	tMemoryStream.w				(&m_tResearchState,	sizeof(m_tResearchState));
	tMemoryStream.w				(&m_tTimeFinish,	sizeof(m_tTimeFinish));
	tMemoryStream.w_string		(m_caDiscoveryToInvestigate);
	{
		tMemoryStream.w_u32		(m_tpOrderedArtefacts.size());
		ARTEFACT_ORDER_IT		I = m_tpOrderedArtefacts.begin();
		ARTEFACT_ORDER_IT		E = m_tpOrderedArtefacts.end();
		for ( ; I != E; ++I) {
			tMemoryStream.w_string((*I).m_caSection);
			tMemoryStream.w_u32	((*I).m_dwCount);
			tMemoryStream.w_u32	((*I).m_dwPrice);
		}
	}
	{
		tMemoryStream.w_u32		(m_tpPurchasedArtefacts.size());
		ITEM_COUNT_PAIR_IT		I = m_tpPurchasedArtefacts.begin();
		ITEM_COUNT_PAIR_IT		E = m_tpPurchasedArtefacts.end();
		for ( ; I != E; ++I) {
			tMemoryStream.w_string((*I).first);
			tMemoryStream.w		(&((*I).second),sizeof((*I).second));
		}
	}
}

void CSE_ALifeOrganization::Load(IReader &tFileStream)
{
	tFileStream.r				(&m_tResearchState,	sizeof(m_tResearchState));
	tFileStream.r				(&m_tTimeFinish,	sizeof(m_tTimeFinish));
	tFileStream.r_string		(m_caDiscoveryToInvestigate);
	{
		m_tpOrderedArtefacts.resize	(tFileStream.r_u32());
		ARTEFACT_ORDER_IT		I = m_tpOrderedArtefacts.begin();
		ARTEFACT_ORDER_IT		E = m_tpOrderedArtefacts.end();
		for ( ; I != E; ++I) {
			tFileStream.r_string((*I).m_caSection);
			(*I).m_dwCount		= tFileStream.r_u32();
			(*I).m_dwPrice		= tFileStream.r_u32();
		}
	}
	{
		u32						l_dwCount = tFileStream.r_u32();
		for (u32 i=0; i<l_dwCount; ++i) {
			string32			S;
			tFileStream.r_string(S);
			LPSTR				l_caArtefactSection = (char*)xr_malloc(32*sizeof(char));
			strcpy				(l_caArtefactSection,S);
			m_tpPurchasedArtefacts.insert(mk_pair(l_caArtefactSection,tFileStream.r_u32()));
		}
	}
}
