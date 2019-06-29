////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_script_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class with script support (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		template <typename _object_type> class _motivation_type,\
		template <typename _object_type> class _motivation_action_type\
	>

#define CSMotivationActionManager CMotivationActionManagerScript<_object_type,_motivation_type,_motivation_action_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationActionManager::CMotivationActionManagerScript	()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
CSMotivationActionManager::~CMotivationActionManagerScript	()
{
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::setup	(_object_type *object)
{
	VERIFY				(object);
	inherited::setup	(object->lua_game_object());
	m_object			= object;
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationActionManager