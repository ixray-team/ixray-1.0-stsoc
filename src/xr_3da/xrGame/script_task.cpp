////////////////////////////////////////////////////////////////////////////
// script_task.сpp :	структура для передачи информации о несюжетных 
//						задания в скрипты
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_task.h"
#include "alife_task.h"
#include "level.h"
#include "xrserver.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_artefact_order.h"

CScriptTask::CScriptTask ()
{
	m_sName = NULL;
	m_iQuantity = 0;
	m_sOrganization = NULL;
	m_iPrice = 0;

}

CScriptTask::~CScriptTask	()
{
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CScriptTaskList::CScriptTaskList	(u16 trader_id)
{
	alife_tasks = xr_new<ALIFE_TASK_VECTOR>();

	CSE_Abstract* E = Level().Server->game->get_entity_from_eid(trader_id);
	CSE_ALifeTrader* pTrader = NULL;
	if(E) pTrader = smart_cast<CSE_ALifeTrader*>(E);

	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator i = pTrader->m_tpOrderedArtefacts.begin();
	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator e = pTrader->m_tpOrderedArtefacts.end();
	for ( ; i != e; ++i) 
	{
		//			Msg     ("Artefact : section[%s] total_count[%d]",(*i).second->m_caSection,(*i).second->m_dwTotalCount);
		//			Msg     ("Orders : ");
		CScriptTask script_task;
		script_task.m_sName = pSettings->r_string((*i).second->m_caSection, "inv_name");

		ALife::ARTEFACT_ORDER_VECTOR::const_iterator    II = (*i).second->m_tpOrders.begin();
		ALife::ARTEFACT_ORDER_VECTOR::const_iterator    EE = (*i).second->m_tpOrders.end();
		for ( ; II != EE; ++II)
		{
			script_task.m_iPrice = (*II).m_price; 
			script_task.m_iQuantity = (*II).m_count;
			shared_str ssss = (*II).m_section;
			script_task.m_sOrganization = pSettings->r_string(ssss, "name");
			alife_tasks->push_back(script_task);
			//Msg("order : section[%s], count[%d], price[%d]",(*II).m_section,(*II).m_count,(*II).m_price);
		}
	}
}

CScriptTaskList::~CScriptTaskList	()
{
	xr_delete(alife_tasks);
}

int	CScriptTaskList::size() const
{
	VERIFY(alife_tasks);
	return alife_tasks->size();
}
CScriptTask	CScriptTaskList::get(int i) const
{
	return (*alife_tasks)[i];
}