////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker setup action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "setup_action.h"

bool CSetupAction::applicable	() const
{
	return							(true);
}

bool CSetupAction::completed	() const
{
	return							(Device.dwTimeGlobal - m_start_time > m_inertia_time);
}

void CSetupAction::initialize	()
{
	m_start_time					= Device.dwTimeGlobal;
	m_animation_action.initialize	();
	m_movement_action.initialize	();
	m_object_action.initialize		();
	m_sight_action.initialize		();
	m_sound_action.initialize		();
}

void CSetupAction::execute		()
{
	m_animation_action.execute		();
	m_movement_action.execute		();
	m_object_action.execute			();
	m_sight_action.execute			();
	m_sound_action.execute			();
}

void CSetupAction::finalize		()
{
	m_animation_action.finalize		();
	m_movement_action.finalize		();
	m_object_action.finalize		();
	m_sight_action.finalize			();
	m_sound_action.finalize			();
}

float CSetupAction::weight		() const
{
	return							(m_weight);
}