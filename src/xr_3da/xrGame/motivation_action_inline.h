////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CSMotivationAction		CMotivationAction<_object_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationAction::CMotivationAction	(const CWorldState &goal) :
	m_goal			(goal)
{
}

TEMPLATE_SPECIALIZATION
IC	const typename CSMotivationAction::CWorldState	&CSMotivationAction::goal	() const
{
	return			(m_goal);
}

TEMPLATE_SPECIALIZATION
void CSMotivationAction::update	()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationAction