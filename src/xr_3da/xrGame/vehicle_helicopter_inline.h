////////////////////////////////////////////////////////////////////////////
//	Module 		: vehicle_helicopter_inline.h
//	Created 	: 13.02.2004
//  Modified 	: 13.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Vehicle helicopter class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CObjectAnimator *CVehicleHelicopter::animator	() const
{
	return					(m_animator);
}

IC	const shared_str &CVehicleHelicopter::animation		() const
{
	return					(m_animation_sting);
}
