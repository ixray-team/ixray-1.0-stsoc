////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human_fsm.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life humans FSM
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "graph_engine.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

//#define OLD_BEHAVIOUR

void CSE_ALifeHumanAbstract::Update			()
{
	if (!bfActive())
		return;
	m_dwTotalMoney		= u32(-1);
#ifdef OLD_BEHAVIOUR
	inherited2::Update	(m_tpALife);
	vfProcessItems		();
#else
	R_ASSERT3			(!m_bOnline,"Can't update online object ",s_name_replace);
	bool				bOk;
	do {
		switch (m_tTaskState) {
			case eTaskStateChooseTask : {
				vfChooseTask();
				break;
			}
			case eTaskStateGoToCustomer : {
				vfGoToCustomer();
				break;
			}
			case eTaskStateGoToSOS : {
				vfGoToSOS();
				break;
			}
			case eTaskStateSendSOS : {
				vfSendSOS();
				break;
			}
			case eTaskStateAccomplishTask : {
				vfAccomplishTask();
				break;
			}
			case eTaskStateSearchItem : {
				vfSearchObject	();
				break;
			}
			default				: NODEFAULT;
		}
		bOk						= bfChooseNextRoutePoint();
		vfProcessItems			();
		m_tpALife->vfCheckForInteraction(this);
		vfCheckForDeletedEvents	();
	}
	while (bOk && bfActive() && (m_tpALife->m_tpActor->o_Position.distance_to(o_Position) > m_tpALife->m_fSwitchDistance));
#endif
	m_tTimeID					= m_tpALife->tfGetGameTime();
}

void CSE_ALifeHumanAbstract::vfChooseTask()
{
	if (!bfHealthIsGood()) {
		if (!bfCanTreat()) {
			if (!bfEnoughMoneyToTreat() || !bfEnoughTimeToTreat()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			//m_tTaskState = eTaskStateHealthCare;
			return;
		}
	}
	else {
		if (!bfEnoughEquipmentToGo()) {
			if (bfDistanceToTraderIsDanger() || !bfEnoughMoneyToEquip()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			m_tDestGraphPointIndex	= _GRAPH_ID(-1);
			vfChooseHumanTask		();
			CSE_ALifeTask			*l_tpTask = m_tpALife->task(m_dwCurTaskID);
			switch (l_tpTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					m_tDestGraphPointIndex = l_tpTask->m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
#pragma todo("Dima to Dima : add graph point type item search")
					//m_baVisitedVertices.assign(tpALife->m_tpTerrain[0][l_tpTask->m_tLocationID].size(),false);
					//m_tDestGraphPointIndex = tpALife->m_tpTerrain[l_tpTask->m_tLocationID][m_dwCurTaskLocation = 0];
					//m_baVisitedVertices[m_dwCurTaskLocation] = true;
					break;
				}
				default				: NODEFAULT;
			};
			m_tTaskState			= eTaskStateAccomplishTask;
			m_tpPath.clear			();
		}
	}
}

void CSE_ALifeHumanAbstract::vfHealthCare()
{
	// if health is low -> use medikit
	// if hunhry -> eat or drink vodka
	// if irradiating -> use antirad or drink vodka 
	// if tired -> sleep, take a rest
	m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeHumanAbstract::vfBuySupplies()
{
	// choose an appropriate trader and go to him to buy supplies
	m_tTaskState = eTaskStateChooseTask;
}

void CSE_ALifeHumanAbstract::vfGoToCustomer()
{
	// go to customer to get something to accomplish task
	if (m_tpPath.empty()) {
		ai().graph_engine().search(ai().game_graph(),m_tGraphID,m_tpALife->object(m_tpALife->task(m_dwCurTaskID)->m_tCustomerID)->m_tGraphID,&m_tpPath,CGraphEngine::CBaseParameters());
		m_dwCurNode					= 0;
		m_tNextGraphID				= m_tGraphID;
		m_fCurSpeed					= m_fGoingSpeed;
	}
	if ((m_dwCurNode >= (m_tpPath.size() - 1)) && (m_tGraphID == m_tNextGraphID)) {
		m_tpPath.clear();
		m_dwCurNode = 0;
		if (int(m_dwCurTaskID) > 0) {
			CSE_ALifeTask			*l_tpTask = m_tpALife->task(m_dwCurTaskID);
			CSE_ALifeTrader			*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(m_tpALife->object(l_tpTask->m_tCustomerID));
			if (l_tpTrader)
				m_tpALife->vfCommunicateWithCustomer(this,l_tpTrader);
		}
		m_tTaskState = eTaskStateChooseTask;
	}
}

void CSE_ALifeHumanAbstract::vfGoToSOS()
{
	// go to person who sent SOS to save or kill
}

void CSE_ALifeHumanAbstract::vfSendSOS()
{
	// send SOS by myself
	m_fCurSpeed						= 0.f;
}

void CSE_ALifeHumanAbstract::vfAccomplishTask()
{
	// build path and wait until we go to the end of it
	m_fCurSpeed		= m_fGoingSpeed;
	m_fProbability	= m_fGoingSuccessProbability; 
	if (m_tpPath.empty()) {
		ai().graph_engine().search(ai().game_graph(),m_tGraphID,m_tDestGraphPointIndex,&m_tpPath,CGraphEngine::CBaseParameters());
		m_dwCurNode				= 0;
		m_tNextGraphID			= m_tGraphID;
	}
	if ((m_dwCurNode + 1 >= (m_tpPath.size())) && (m_tGraphID == m_tNextGraphID)) {
		if (bfCheckIfTaskCompleted()) {
			m_tpPath.clear		();
			m_tTaskState		= eTaskStateGoToCustomer;
		}
		else {
			CSE_ALifeTask		*l_tpALifeTask = m_tpALife->task(m_dwCurTaskID);
			switch (l_tpALifeTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					if ((m_dwCurNode + 1>= (m_tpPath.size())) && (m_tGraphID == m_tNextGraphID)) {
						++(l_tpALifeTask->m_dwTryCount);
						m_tTaskState = eTaskStateChooseTask;
					}
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
//					++(tpALife->task(m_dwCurTaskID)->m_dwTryCount);
//					m_tTaskState		= eTaskStateChooseTask;
//					for (++m_dwCurTaskLocation; (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) && (m_baVisitedVertices[m_dwCurTaskLocation]); ++m_dwCurTaskLocation);
//					if (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) {
//						m_baVisitedVertices[m_dwCurTaskLocation] = true;
//						tpAlife->ffFindMinimalPath(m_tGraphID,tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID][m_dwCurTaskLocation],m_tpPath);
//						m_dwCurNode = 0;
//					}
//					else
						m_tTaskState	= eTaskStateChooseTask;
					break;
				}
				default :				NODEFAULT;
			}
		}
	}
	else
		if (m_tNextGraphID == m_tpALife->task(m_dwCurTaskID)->m_tGraphID)
			m_tTaskState	= eTaskStateSearchItem;
}

void CSE_ALifeHumanAbstract::vfSearchObject()
{
	m_fCurSpeed		= m_fSearchSpeed;
	m_fProbability	= m_fSearchSuccessProbability; 
	CSE_ALifeTask	*l_tpALifeTask = m_tpALife->task(m_dwCurTaskID);
	switch (l_tpALifeTask->m_tTaskType) {
		case eTaskTypeSearchForItemCG :
		case eTaskTypeSearchForItemOG : {
			if ((m_tNextGraphID != l_tpALifeTask->m_tGraphID) || (m_tGraphID == m_tpALife->task(m_dwCurTaskID)->m_tGraphID))
				m_tTaskState	= eTaskStateAccomplishTask;
			break;
		}
		case eTaskTypeSearchForItemCL :
		case eTaskTypeSearchForItemOL : {
			m_tTaskState	= eTaskStateChooseTask;
			break;
		}
		default :				NODEFAULT;
	}
}