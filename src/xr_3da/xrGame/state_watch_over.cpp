////////////////////////////////////////////////////////////////////////////
//	Module 		: state_watch_over.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Watch over state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_watch_over.h"
#include "ai/stalker/ai_stalker.h"

using namespace MonsterSpace;

CStateWatchOver::CStateWatchOver	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateWatchOver::~CStateWatchOver	()
{
}

void CStateWatchOver::Init			()
{
}

void CStateWatchOver::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateWatchOver::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateWatchOver::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateWatchOver::initialize		()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskHumming));
	set_inertia_time				(::Random.randI(5,10)*1000);
	m_start_yaw						= m_object->Orientation().yaw;
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
}

void CStateWatchOver::execute			()
{
	Fvector							direction;
	direction.setHP					(m_start_yaw,0);
	m_object->setup					(SightManager::eSightTypeDirection,&direction);
	
	m_object->set_path_type			(CMovementManager::ePathTypeNoPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeStand);
	m_object->set_mental_state		(eMentalStateDanger);
}

void CStateWatchOver::finalize		()
{
	inherited::finalize				();
}
