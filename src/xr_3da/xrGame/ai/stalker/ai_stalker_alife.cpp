////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife.cpp
//	Created 	: 25.07.2003
//  Modified 	: 25.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife online behaviour
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../alife_simulator.h"
#include "../../alife_task_registry.h"
#include "../../graph_engine.h"
#include "../../xrserver_objects_alife.h"

void CAI_Stalker::vfChooseTask()
{
	if (!bfHealthIsGood()) {
		if (!bfCanTreat()) {
			if (!bfEnoughMoneyToTreat() || !bfEnoughTimeToTreat()) {
				m_tTaskState = ALife::eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = ALife::eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			vfChooseTask();
			return;
		}
	}
	else {
		if (!bfEnoughEquipmentToGo()) {
			if (bfDistanceToTraderIsDanger() || !bfEnoughMoneyToEquip()) {
				m_tTaskState = ALife::eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = ALife::eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			m_tDestGraphPointIndex	= ALife::_GRAPH_ID(-1);
			vfChooseHumanTask		();
			CALifeTask			*l_tpTask = ai().alife().tasks().task(m_tTaskID);
			switch (l_tpTask->m_tTaskType) {
				case ALife::eTaskTypeSearchForItemCG :
				case ALife::eTaskTypeSearchForItemOG : {
					m_tDestGraphPointIndex = l_tpTask->m_tGraphID;
					break;
				}
				case ALife::eTaskTypeSearchForItemCL :
				case ALife::eTaskTypeSearchForItemOL : {
#pragma todo("Dima to Dima : add graph point type item search")
					//m_baVisitedVertices.assign(tpALife->m_tpTerrain[0][l_tpTask->m_tLocationID].size(),false);
					//m_tDestGraphPointIndex = tpALife->m_tpTerrain[l_tpTask->m_tLocationID][m_dwCurTaskLocation = 0];
					//m_baVisitedVertices[m_dwCurTaskLocation] = true;
					break;
				}
				default				: NODEFAULT;
			};
			m_tTaskState			= ALife::eTaskStateAccomplishTask;
		}
	}
}

void CAI_Stalker::vfGoToCustomer()
{
//	if (m_tpGraphPath.empty()) {
//		ai().m_tpAStar->ffFindMinimalPath(m_tGraphID,m_tpALife->object(m_tpALife->task(m_tTaskID)->m_tCustomerID)->m_tGraphID,m_tpGraphPath);
//		m_dwCurGraphPathNode					= 0;
//		m_tNextGraphID				= m_tGraphID;
//	}
//	if ((m_dwCurGraphPathNode >= (m_tpGraphPath.size() - 1)) && (m_tGraphID == m_tNextGraphID)) {
//		m_tpGraphPath.clear();
//		m_dwCurGraphPathNode = 0;
//		if (int(m_tTaskID) > 0) {
////			CALifeTask			*l_tpTask = m_tpALife->task(m_tTaskID);
////			CSE_ALifeTrader			*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(m_tpALife->object(l_tpTask->m_tCustomerID));
////			if (l_tpTrader)
////				m_tpALife->communicate_with_customer(this,l_tpTrader);
//		}
//		m_tTaskState = eTaskStateChooseTask;
//	}
}

void CAI_Stalker::vfAccomplishTask()
{
//	// build path and wait until we go to the end of it
//	if (m_tpGraphPath.empty() && (m_tGraphID != m_tDestGraphPointIndex)) {
//		ai().m_tpAStar->ffFindMinimalPath(m_tGraphID,m_tDestGraphPointIndex,m_tpGraphPath);
//		m_dwCurGraphPathNode	= 0;
//		m_tNextGraphID			= m_tGraphID;
//	}
//
//	if (m_tNextGraphID == m_tpALife->task(m_tTaskID)->m_tGraphID)
//		m_tTaskState	= eTaskStateSearchItem;
//	else
//		m_tTaskState	= eTaskStateGoingToSearchItem;
}

void CAI_Stalker::vfFinishTask()
{
	if (bfCheckIfTaskCompleted())
		m_tTaskState		= ALife::eTaskStateGoToCustomer;
	else {
		CALifeTask		*l_tpALifeTask = ai().alife().tasks().task(m_tTaskID);
		switch (l_tpALifeTask->m_tTaskType) {
			case ALife::eTaskTypeSearchForItemCG :
			case ALife::eTaskTypeSearchForItemOG : {
				if (CMovementManager::path_completed() && (game_vertex_id() == l_tpALifeTask->m_tGraphID)) {
					++(l_tpALifeTask->m_dwTryCount);
					m_tTaskState = ALife::eTaskStateChooseTask;
				}
				break;
			}
			case ALife::eTaskTypeSearchForItemCL :
			case ALife::eTaskTypeSearchForItemOL : {
//					++(tpALife->task(m_tTaskID)->m_dwTryCount);
//					m_tTaskState		= eTaskStateChooseTask;
//					for (++m_dwCurTaskLocation; (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) && (m_baVisitedVertices[m_dwCurTaskLocation]); ++m_dwCurTaskLocation);
//					if (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) {
//						m_baVisitedVertices[m_dwCurTaskLocation] = true;
//						tpAlife->ffFindMinimalPath(m_tGraphID,tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID][m_dwCurTaskLocation],m_tpGraphPath);
//						m_dwCurGraphPathNode = 0;
//					}
//					else
				m_tTaskState	= ALife::eTaskStateChooseTask;
				break;
			}
			default :				NODEFAULT;
		}
	}
}

void CAI_Stalker::vfGoToSOS()
{
}

void CAI_Stalker::vfSendSOS()
{
}

void CAI_Stalker::ALifeUpdate()
{
	if (!ai().get_alife()) {
		vfContinueWithALifeGoals();
		return;
	}
	switch (m_tTaskState) {
		case ALife::eTaskStateChooseTask : {
			vfChooseTask();
			break;
		}
		case ALife::eTaskStateGoToCustomer : {
			vfGoToCustomer();
			break;
		}
		case ALife::eTaskStateGoToSOS : {
			vfGoToSOS();
			break;
		}
		case ALife::eTaskStateSendSOS : {
			vfSendSOS();
			break;
		}
		case ALife::eTaskStateAccomplishTask : {
			vfAccomplishTask();
			break;
		}
		case ALife::eTaskStateSearchItem : {
			vfSearchObject();
			break;
		}
		case ALife::eTaskStateGoingToCustomer :
		case ALife::eTaskStateGoingToSearchItem : {
			vfContinueWithALifeGoals();
			break;
		}
		default			: NODEFAULT;
	}
}