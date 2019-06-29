////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_organization_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife organization registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_organization_registry.h"
#include "alife_organization.h"
#include "alife_discovery.h"
#include "object_broker.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_artefact_order.h"

using namespace ALife;

struct COrganizationLoader : public object_loader::detail::CEmptyPredicate {
	using object_loader::detail::CEmptyPredicate::operator();

	bool				m_load;
	mutable ALife::ORGANIZATION_P_MAP::iterator I;
	ALife::ORGANIZATION_P_MAP::iterator E;
	
	IC	COrganizationLoader(ALife::ORGANIZATION_P_MAP &orgs, bool load) : m_load(load)
	{
		I = orgs.begin();
		E = orgs.end();
	}

	IC	bool can_clear() const {return(false);}
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value, bool first) const {return(!m_load && !first);}
	template <typename T1, typename T2>
	IC	void after_load	(T1 &data, T2 &stream) const
	{
		data.second		= (*I).second;
		load_data		(*data.second,stream);
		VERIFY2			(I != E,"Data key value mismatch : DELETE saved game and try again!");
	}

	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, T2 &value) const
	{
		if (!m_load)
			return		(true);
		(*I).second = value.second;
		++I;
		return			(false);
	}
};

struct CDiscoveryLoader : public object_loader::detail::CEmptyPredicate {
	using object_loader::detail::CEmptyPredicate::operator();

	bool				m_load;
	mutable ALife::DISCOVERY_P_MAP::iterator I;
	ALife::DISCOVERY_P_MAP::iterator E;
	
	IC	CDiscoveryLoader(ALife::DISCOVERY_P_MAP &discoveries, bool load) : m_load(load)
	{
		I = discoveries.begin();
		E = discoveries.end();
	}

	IC	bool can_clear() const {return(false);}
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value, bool first) const {return(!m_load && !first);}
	template <typename T1, typename T2>
	IC	void after_load	(T1 &data, T2 &stream) const
	{
		data.second		= (*I).second;
		load_data		(*data.second,stream);
		VERIFY2			(I != E,"Data key value mismatch : DELETE saved game and try again!");
	}

	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, T2 &value) const
	{
		if (!m_load)
			return		(true);
		(*I).second = value.second;
		++I;
		return			(false);
	}
};

class CArtefactOrderPredicate {
public:
	IC	bool		operator()	(const SArtefactOrder &tArtefactOrder1, const SArtefactOrder &tArtefactOrder2) const
	{
		return		(tArtefactOrder1.m_price > tArtefactOrder2.m_price);
	};
};

CALifeOrganizationRegistry::CALifeOrganizationRegistry	(LPCSTR section)
{
	seed						(u32(CPU::QPC() & 0xffffffff));
	R_ASSERT2					(pSettings->section_exist("organizations"),"There is no section 'organizations' in the 'system.ltx'!");
	LPCSTR						N,V;
	for (u32 k = 0; pSettings->r_line("organizations",k,&N,&V); ++k)
		m_organizations.insert	(mk_pair(N,xr_new<CALifeOrganization>(N)));

	ORGANIZATION_P_MAP::iterator		I = m_organizations.begin();
	ORGANIZATION_P_MAP::iterator		E = m_organizations.end();
	for ( ; I != E; ++I) {
		LPSTR_VECTOR::const_iterator	i = (*I).second->possible_discoveries().begin();
		LPSTR_VECTOR::const_iterator	e = (*I).second->possible_discoveries().end();
		for ( ; i != e; ++i) {
			DISCOVERY_P_MAP::iterator	j = m_discoveries.find(*i);
			if (m_discoveries.end() == j)
				m_discoveries.insert	(mk_pair(*i,xr_new<CALifeDiscovery>(*i)));
		}
	}
	LPCSTR						S = pSettings->r_string("alife","preknown_artefacts");
	for (u32 i=0, n=_GetItemCount(S); i<n; ++i) {
		string64				S1;
		_GetItem				(S,i,S1);
		LPSTR					S2 = xr_alloc<char>(xr_strlen(S1) + 1);
		strcpy					(S2,S1);
		m_artefacts.insert		(mk_pair(S2,false));
	}
}

CALifeOrganizationRegistry::~CALifeOrganizationRegistry	()
{
	delete_data					(m_organizations);
	delete_data					(m_discoveries);
	delete_data					(m_artefacts);
}

void CALifeOrganizationRegistry::save					(IWriter &memory_stream)
{
	Msg							("* Saving organizations and discoveries...");
	memory_stream.open_chunk	(DISCOVERY_CHUNK_DATA);
	save_data					(m_organizations,	memory_stream,COrganizationLoader(m_organizations,false));
	save_data					(m_discoveries,		memory_stream,CDiscoveryLoader(m_discoveries,false));
	save_data					(m_artefacts,		memory_stream);
	memory_stream.close_chunk	();
}

void CALifeOrganizationRegistry::load					(IReader &file_stream)
{ 
	Msg							("* Loading organizations and discoveries...");
	R_ASSERT2					(file_stream.find_chunk(DISCOVERY_CHUNK_DATA),"Can't find chunk DISCOVERY_CHUNK_DATA!");
	load_data					(m_organizations,	file_stream,COrganizationLoader(m_organizations,true));
	load_data					(m_discoveries,		file_stream,CDiscoveryLoader(m_discoveries,true));
	load_data					(m_artefacts,		file_stream);
}

void CALifeOrganizationRegistry::update()
{
	// iterating on all the organizations
	ALife::ORGANIZATION_P_PAIR_IT	I = m_organizations.begin();
	ALife::ORGANIZATION_P_PAIR_IT	E = m_organizations.end();
	for ( ; I != E; ++I) {
		switch ((*I).second->m_research_state) {
			case ALife::eResearchStateLeft : {
				// asking if we have to join zone investigations
				if (randF(1) >= (*I).second->m_join_probability)
					break;

				(*I).second->m_research_state = ALife::eResearchStateJoin;
			}
			case ALife::eResearchStateJoin : {
				// asking if we have to left zone investigations
				if (!xr_strlen((*I).second->m_discovery_to_investigate) && (*I).second->m_ordered_artefacts.empty() && (randF(1) < (*I).second->m_left_probability))
					(*I).second->m_research_state = ALife::eResearchStateLeft;
				else {
					// selecting discovery we would like to investigate
					CALifeDiscovery		*l_tpBestDiscovery = 0;
					bool				l_bGoToResearch = false;
					LPSTR_IT			i = (*I).second->m_possible_discoveries.begin();
					LPSTR_IT			e = (*I).second->m_possible_discoveries.end();
					for ( ; i != e; ++i) {
						// getting pointer to discovery object from the discovery registry
						ALife::DISCOVERY_P_PAIR_IT	j = m_discoveries.find(*i);
						R_ASSERT2					(m_discoveries.end() != j,"Invalid discovery name in the possible iscoveries parameters in the 'system.ltx'!");

						// checking if discovery has not been invented yet
						if ((*j).second->m_invented)
							continue;

						// checking if discovery depends on the non-invented discoveries
						LPSTR_IT					II = (*j).second->m_dependency.begin();
						LPSTR_IT					EE = (*j).second->m_dependency.end();
						bool						l_bFoundDiscovery = true;
						for ( ; II != EE; ++II) {
							ALife::DISCOVERY_P_PAIR_IT	J = m_discoveries.find(*II);
							R_ASSERT2					(m_discoveries.end() != J,"Invalid discovery dependency!");
							if (!(*J).second->m_invented) {
								l_bFoundDiscovery = false;
								break;
							}
						}
						if (!l_bFoundDiscovery)
							break;

						// checking if artefacts being used during discovery invention are known
						// and there are enough artefacts purchased for discovery
						ALife::ARTEFACT_ORDER_IT	ii = (*j).second->m_query.begin();
						ALife::ARTEFACT_ORDER_IT	ee = (*j).second->m_query.end();
						bool						l_bIsReadyForInvention = true;
						for ( ; ii != ee; ++ii) {
							ALife::LPSTR_BOOL_PAIR_IT	iii = m_artefacts.find((LPSTR)*(*ii).m_section);
							R_ASSERT3					(m_artefacts.end() != iii,"Unknown artefact",*(*ii).m_section);
							if ((*iii).second) {
								l_bFoundDiscovery	= false;
								break;
							}
							else {
								ALife::ITEM_COUNT_PAIR_IT	jj = (*I).second->m_purchased_artefacts.find((LPSTR)*(*ii).m_section);
								if (((*I).second->m_purchased_artefacts.end() == jj) || ((*jj).second < (*ii).m_count))
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
						strcpy((*I).second->m_discovery_to_investigate,  l_tpBestDiscovery->m_id);
						// checking if we are ready to invent it
						if (l_bGoToResearch) {
							(*I).second->m_research_state = ALife::eResearchStateResearch;
							(*I).second->m_purchased_artefacts.clear();
						}
						else {
							// if not - order artefacts needed for the discovery
							(*I).second->m_ordered_artefacts = l_tpBestDiscovery->m_query;
						}
					}
					else
						(*I).second->m_purchased_artefacts.clear();
				}
				break;
			}
			case ALife::eResearchStateResearch : {
				// getting pointer to discovery object from the discovery registry
				ALife::DISCOVERY_P_PAIR_IT	i = m_discoveries.find((*I).second->m_discovery_to_investigate);
				R_ASSERT2(m_discoveries.end() != i,"Unknown discovery!");

				// checking if we've got any result
				if (randF(1) < (*i).second->m_result_probability) {
					// checking if we've invented the discovery
					if (randF(1) < (*i).second->m_success_probability) {
						R_ASSERT2(!(*i).second->m_invented,"Discovery has been already invented!");
						(*i).second->m_invented = true;
						(*I).second->m_research_state = ALife::eResearchStateJoin;
						// ordering artefacts
						ALife::DEMAND_P_IT	ii = (*i).second->m_demands.begin();
						ALife::DEMAND_P_IT	ee = (*i).second->m_demands.end();
						(*I).second->m_ordered_artefacts.resize(ee - ii);
						ALife::ARTEFACT_ORDER_IT		II = (*I).second->m_ordered_artefacts.begin();
						for ( ; ii != ee; ++ii, ++II) {
							(*II).m_section = (*ii)->m_section;
							(*II).m_count	= randI((*ii)->min_count(),(*ii)->max_count());
							(*II).m_price	= randI((*ii)->min_price(),(*ii)->max_price());
						}
					}
					else
						// checking if we've destroyed laboratory during investigations
						if (randF(1) < (*i).second->m_destroy_probability) {
							(*I).second->m_research_state = ALife::eResearchStateFreeze;
						}
						else {
							// otherwise - we finished with investigations without any discovery;
							(*I).second->m_research_state = ALife::eResearchStateJoin;
						}
				}
				break;
			}
			case ALife::eResearchStateFreeze : {
				// getting pointer to discovery object from the discovery registry
				ALife::DISCOVERY_P_PAIR_IT	i = m_discoveries.find((*I).second->m_discovery_to_investigate);
				R_ASSERT2					(m_discoveries.end() != i,"Unknown discovery!");
				// checking if we've restored
				if (randF(1) < (*i).second->m_unfreeze_probability)
					(*I).second->m_research_state = ALife::eResearchStateJoin;
				break;
			}
			default : NODEFAULT;
		}
	}
}

void CALifeOrganizationRegistry::update_artefact_orders(CSE_ALifeTrader &tTrader)
{
	// iterating on all the organizations
	ORGANIZATION_P_PAIR_IT	I = m_organizations.begin();
	ORGANIZATION_P_PAIR_IT	E = m_organizations.end();
	for ( ; I != E; ++I) {
		// checking if our rank is enough for the organization
		// and the organization in the appropriate state
		if (((*I).second->m_id == tTrader.m_tOrgID) && (eResearchStateJoin == (*I).second->m_research_state)) {
			ARTEFACT_ORDER_IT		i = (*I).second->m_ordered_artefacts.begin();
			ARTEFACT_ORDER_IT		e = (*I).second->m_ordered_artefacts.end();
			for ( ; i != e; ++i) {
				ARTEFACT_TRADER_ORDER_PAIR_IT	ii = tTrader.m_tpOrderedArtefacts.find(*(*i).m_section);
				if (tTrader.m_tpOrderedArtefacts.end() != ii) {
					(*ii).second->m_dwTotalCount += (*i).m_count;
					SArtefactOrder	l_tArtefactOrder;
					l_tArtefactOrder.m_count	= (*i).m_count;
					l_tArtefactOrder.m_price	= (*i).m_price;
					l_tArtefactOrder.m_section	= (*I).first;
					(*ii).second->m_tpOrders.push_back(l_tArtefactOrder);
				}
				else {
					SArtefactTraderOrder	*l_tpTraderArtefactOrder = xr_new<SArtefactTraderOrder>();
					l_tpTraderArtefactOrder->m_caSection = (*i).m_section;
					l_tpTraderArtefactOrder->m_dwTotalCount = (*i).m_count;
					SArtefactOrder			l_tArtefactOrder;
					l_tArtefactOrder.m_count	= (*i).m_count;
					l_tArtefactOrder.m_price	= (*i).m_price;
					l_tArtefactOrder.m_section	= (*I).first;
					l_tpTraderArtefactOrder->m_tpOrders.push_back(l_tArtefactOrder);
					tTrader.m_tpOrderedArtefacts.insert(mk_pair(*l_tpTraderArtefactOrder->m_caSection,l_tpTraderArtefactOrder));
					R_ASSERT				(tTrader.m_tpOrderedArtefacts.find(*l_tpTraderArtefactOrder->m_caSection) != tTrader.m_tpOrderedArtefacts.end());
					// updating cross traders table
					TRADER_SET_PAIR_IT		J = m_cross_traders.find(*(*i).m_section);
					if (m_cross_traders.end() == J) {
						m_cross_traders.insert(mk_pair(*(*i).m_section,TRADER_SET()));
						J = m_cross_traders.find(*(*i).m_section);
						R_ASSERT2	(m_cross_traders.end() != J,"Cannot append the cross trader map!");
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
