////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_discovery.cpp
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife discovery class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_discovery.h"
#include "object_broker.h"
#include "alife_artefact_order.h"

using namespace ALife;

CALifeDiscovery::CALifeDiscovery	()
{
	R_ASSERT2					(false,"Default constructor cannot be called explicitly!");
}

CALifeDiscovery::CALifeDiscovery	(LPCSTR section)
{
	m_id						= pSettings->r_string	(section,"name");
	m_success_probability		= pSettings->r_float	(section,"success_probability");
	m_destroy_probability		= pSettings->r_float	(section,"destroy_probability");
	m_result_probability		= pSettings->r_float	(section,"result_probability");
	m_unfreeze_probability		= pSettings->r_float	(section,"unfreeze_probability");
	m_invented					= !!pSettings->r_bool	(section,"already_invented");

	LPCSTR						S;
	string64					S1;
	{
		S						= pSettings->r_string	(section,"demand_on_success");
		R_ASSERT2				(!(_GetItemCount(S) % 5),"Invalid argument count in the discovery object section!");
		m_demands.resize			(_GetItemCount(S)/5);
		DEMAND_P_IT				B = m_demands.begin(), I = B;
		DEMAND_P_IT				E = m_demands.end();
		for ( ; I != E; ++I)
			*I					= xr_new<CALifeArtefactDemand>(_GetItem(S,5*int(I - B) + 0,S1),atoi(_GetItem(S,5*int(I - B) + 1,S1)),atoi(_GetItem(S,5*int(I - B) + 2,S1)),atoi(_GetItem(S,5*int(I - B) + 3,S1)),atoi(_GetItem(S,5*int(I - B) + 4,S1)));
	}
	{
		S						= pSettings->r_string	(section,"artefacts");
		R_ASSERT2				(!(_GetItemCount(S) % 3),"Invalid argument count in the discovery object section!");
		u32						count = _GetItemCount(S)/3;
		for (u32 i=0; i<count; ++i) {
			SArtefactOrder		order;
			string256			temp_sect;			
			_GetItem			(S,3*i + 0,temp_sect);	order.m_section=temp_sect;
			order.m_count		= atoi(_GetItem(S,3*i + 1,S1));
			order.m_price		= atoi(_GetItem(S,3*i + 2,S1));
			m_query.push_back	(order);
		}
	}
	{
		S						= pSettings->r_string	(section,"discovery_dependence");
		m_dependency.resize		(_GetItemCount(S));
		LPSTR_IT				B = m_dependency.begin(), I = B;
		LPSTR_IT				E = m_dependency.end();
		for ( ; I != E; ++I) {
			_GetItem			(S,int(I - B),S1);
			*I					= xr_alloc<char>(xr_strlen(S1) + 1);
			strcpy				((char*)(*I),S1);
		}
	}
}

CALifeDiscovery::~CALifeDiscovery	()
{
	delete_data					(m_demands);
	delete_data					(m_dependency);
}

void CALifeDiscovery::save			(IWriter &tMemoryStream)
{
	tMemoryStream.w_u32			(u32(m_invented));
}

void CALifeDiscovery::load			(IReader &tFileStream)
{
	m_invented					= !!tFileStream.r_u32();
}
