////////////////////////////////////////////////////////////////////////////
//	Module 		: state_back_enemy_check.cpp
//	Created 	: 10.02.2004
//  Modified 	: 10.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Back enemy check state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_back_enemy_check.h"
#include "ai/stalker/ai_stalker.h"

using namespace MonsterSpace;

CStateBackEnemyCheck::CStateBackEnemyCheck	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateBackEnemyCheck::~CStateBackEnemyCheck	()
{
}

void CStateBackEnemyCheck::Init			()
{
}

void CStateBackEnemyCheck::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateBackEnemyCheck::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateBackEnemyCheck::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateBackEnemyCheck::initialize		()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskHumming));
	set_inertia_time				(::Random.randI(10,30)*1000);
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
}

void CStateBackEnemyCheck::execute			()
{
	m_object->set_path_type			(CMovementManager::ePathTypeNoPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeStand);
	m_object->set_mental_state		(eMentalStateDanger);
}

void CStateBackEnemyCheck::finalize		()
{
	inherited::finalize				();
}
