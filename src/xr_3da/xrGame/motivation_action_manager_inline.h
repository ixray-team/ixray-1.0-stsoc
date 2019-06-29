////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		template <typename _object_type> class _motivation_type,\
		template <typename _object_type> class _motivation_action_type\
	>

#define CSMotivationActionManager CMotivationActionManager<_object_type,_motivation_type,_motivation_action_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivationActionManager::CMotivationActionManager	()
{
}

TEMPLATE_SPECIALIZATION
CSMotivationActionManager::~CMotivationActionManager	()
{
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::setup	(_object_type *object)
{
	CSMotivationManager::setup	(object);
	CSActionPlanner::setup		(object);
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::update	()
{
	CSMotivationManager::update			();
	CSActionPlanner::set_target_state	(CSMotivationManager::selected()->goal());
	CSActionPlanner::update				();
}

TEMPLATE_SPECIALIZATION
void CSMotivationActionManager::clear	()
{
	CSMotivationManager::clear			();
	CSActionPlanner::clear				();
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationActionManager::clear_motivations	()
{
	CSMotivationManager::clear			();
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationActionManager::clear_actions	()
{
	CSActionPlanner::clear				();
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationActionManager::add_condition	(CWorldState &goal, _condition_type condition_id, _value_type value)
{
	goal.add_condition					(CWorldProperty(condition_id,value));
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivationActionManager::add_action		(const _edge_type &a, CScriptActionPlannerAction *b)
{
	CSActionPlanner::add_operator		(a,b);
}

TEMPLATE_SPECIALIZATION
IC	_object_type *CSMotivationActionManager::object	() const
{
	VERIFY								(CSActionPlanner::m_object);
	return								(CSActionPlanner::m_object);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivationActionManager