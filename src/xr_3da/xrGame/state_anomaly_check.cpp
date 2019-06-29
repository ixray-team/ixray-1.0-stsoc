////////////////////////////////////////////////////////////////////////////
//	Module 		: state_anomaly_check.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Anomaly check state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_anomaly_check.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

using namespace MonsterSpace;

CStateAnomalyCheck::CStateAnomalyCheck	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateAnomalyCheck::~CStateAnomalyCheck	()
{
}

void CStateAnomalyCheck::Init			()
{
}

void CStateAnomalyCheck::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateAnomalyCheck::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateAnomalyCheck::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateAnomalyCheck::initialize		()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskHumming));
	set_inertia_time				(::Random.randI(5,10)*1000);
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
}

void CStateAnomalyCheck::execute			()
{
	m_object->set_path_type			(CMovementManager::ePathTypeNoPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeStand);
	m_object->set_mental_state		(eMentalStateDanger);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionFire1,m_object->inventory().m_slots[5].m_pIItem);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionFire1,m_object->inventory().m_slots[5].m_pIItem);
#endif
}

void CStateAnomalyCheck::finalize		()
{
	inherited::finalize				();
}
