////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker setup action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSetupAction::CSetupAction				(float weight, u32 inertia_time) :
	m_weight						(weight),
	m_inertia_time					(inertia_time)
{
}

IC	void CSetupAction::set_object			(CAI_Stalker *object)
{
	VERIFY							(object);
	m_object						= object;
	m_animation_action.set_object	(object);
	m_movement_action.set_object	(object);
	m_object_action.set_object		(object);
	m_sight_action.set_object		(object);
	m_sound_action.set_object		(object);
}

IC	void CSetupAction::animation	(const CAnimationAction &animation_action)
{
	m_animation_action				= animation_action;
}

IC	void CSetupAction::movement	(const CMovementAction &movement_action)
{
	m_movement_action				= movement_action;
}

IC	void CSetupAction::object		(const CObjectAction &object_action)
{
	m_object_action					= object_action;
}

IC	void CSetupAction::sight		(const CSightAction	&sight_action)
{
	m_sight_action					= sight_action;
}

IC	void CSetupAction::sound		(const CSoundAction	&sound_action)
{
	m_sound_action					= sound_action;
}

IC	void CSetupAction::update				(const CSetupAction &setup_action)
{
	animation						(setup_action.animation());
	movement						(setup_action.movement());
	object							(setup_action.object());
	sight							(setup_action.sight());
	sound							(setup_action.sound());
	set_object						(m_object);
}

IC	const CAnimationAction &CSetupAction::animation	() const
{
	return							(m_animation_action);
}

IC	const CMovementAction &CSetupAction::movement	() const
{
	return							(m_movement_action);
}

IC	const CObjectAction &CSetupAction::object		() const
{
	return							(m_object_action);
}

IC	const CSightAction &CSetupAction::sight			() const
{
	return							(m_sight_action);
}

IC	const CSoundAction &CSetupAction::sound			() const
{
	return							(m_sound_action);
}

IC	CAnimationAction &CSetupAction::animation		()
{
	return							(m_animation_action);
}

IC	CMovementAction &CSetupAction::movement			()
{
	return							(m_movement_action);
}

IC	CObjectAction &CSetupAction::object				()
{
	return							(m_object_action);
}

IC	CSightAction &CSetupAction::sight				()
{
	return							(m_sight_action);
}

IC	CSoundAction &CSetupAction::sound				()
{
	return							(m_sound_action);
}
