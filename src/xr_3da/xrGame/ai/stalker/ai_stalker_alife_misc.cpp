////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife_misc.cpp
//	Created 	: 15.09.2003
//  Modified 	: 15.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../alife_simulator.h"
#include "../../alife_task_registry.h"
#include "../../graph_engine.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "../../inventory.h"
#include "../../alife_task.h"

bool CAI_Stalker::bfHealthIsGood			()
{
	return(true);
}

bool CAI_Stalker::bfItemCanTreat			(CInventoryItem *tpInventoryItem)
{
	return(false);
}

void CAI_Stalker::vfUseItem					(CInventoryItem *tpInventoryItem)
{
}

bool CAI_Stalker::bfCanTreat				()
{
//	xr_vector<u16>::iterator	I = children.begin();
//	xr_vector<u16>::iterator	E = children.end();
//	for ( ; I != E; ++I) {
//		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(m_tpALife->object(*I));
//		R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item ihas a parent?!");
//		if (bfItemCanTreat(l_tpALifeInventoryItem))
//			vfUseItem			(l_tpALifeInventoryItem);
//	}
	return						(bfHealthIsGood());
}

bool CAI_Stalker::bfEnoughMoneyToTreat		()
{
	return(true);
}

bool CAI_Stalker::bfEnoughTimeToTreat		()
{
	return(true);
}

bool CAI_Stalker::bfEnoughEquipmentToGo	()
{
	return(true);
}

bool CAI_Stalker::bfDistanceToTraderIsDanger()
{
	return(false);
}

bool CAI_Stalker::bfEnoughMoneyToEquip		()
{
	return(true);
}

bool CAI_Stalker::bfCheckIfTaskCompleted()
{
	if (int(m_tTaskID) < 0)
		return		(false);

	CALifeTask	&tTask = *ai().alife().tasks().task(m_tTaskID);
	switch (tTask.m_tTaskType) {
		case ALife::eTaskTypeSearchForItemCL :
		case ALife::eTaskTypeSearchForItemCG : {
			if (inventory().dwfGetSameItemCount(tTask.m_caSection))
				return(true);
			break;
		}
		case ALife::eTaskTypeSearchForItemOL :
		case ALife::eTaskTypeSearchForItemOG : {
			if (inventory().bfCheckForObject(tTask.m_tObjectID))
				return(true);
			break;
		}
	}
	return			(false);
}

void CAI_Stalker::vfChooseHumanTask()
{
	ALife::OBJECT_IT			I = m_tpKnownCustomers.begin();
	ALife::OBJECT_IT			E = m_tpKnownCustomers.end();
	for ( ; I != E; ++I) {
		ALife::OBJECT_TASK_MAP::const_iterator	J = ai().alife().tasks().cross().find(*I);
		R_ASSERT2				(ai().alife().tasks().cross().end() != J,"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");

		u32						l_dwMinTryCount = u32(-1);
		ALife::_TASK_ID			l_tBestTaskID = ALife::_TASK_ID(-1);
		ALife::TASK_SET::const_iterator		i = (*J).second.begin();
		ALife::TASK_SET::const_iterator		e = (*J).second.end();
		for ( ; i != e; ++i) {
			CALifeTask		*l_tpTask = ai().alife().tasks().task(*i);
			if (!l_tpTask->m_dwTryCount) {
				l_tBestTaskID = l_tpTask->m_tTaskID;
				break;
			}
			else
				if (l_tpTask->m_dwTryCount < l_dwMinTryCount) {
					l_dwMinTryCount = l_tpTask->m_dwTryCount;
					l_tBestTaskID = l_tpTask->m_tTaskID;
				}
		}

		if (ALife::_TASK_ID(-1) != l_tBestTaskID) {
			vfSetCurrentTask	(l_tBestTaskID);
			break;
		}
	}
}

void CAI_Stalker::vfSetCurrentTask(ALife::_TASK_ID &tTaskID)
{
	m_tTaskID				= ai().alife().tasks().task(tTaskID)->m_tTaskID;
}

bool CAI_Stalker::bfAssignDestinationNode()
{
//	if ((level_vertex_id() == m_level_dest_vertex_id) && (ai().cross_table().vertex(level_vertex_id()) == game_dest_vertex_id())) {
	if (ai().get_alife())
		return				(!CMovementManager::path_completed());

	if (ai().game_graph().vertex(game_dest_vertex_id())->level_vertex_id() != level_vertex_id()) {
		set_level_dest_vertex(ai().game_graph().vertex(game_dest_vertex_id())->level_vertex_id());
		return				(true);
	}

	u32						vertex_id;
	select_random_location	(game_vertex_id(),vertex_id);
	set_game_dest_vertex	(ALife::_GRAPH_ID(vertex_id));
	return					(true);
}
