////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_effect_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator effect inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CAbstractOperatorEffect COperatorEffectAbstract<T2,T3>

TEMPLATE_SPECIALIZATION
IC	CAbstractOperatorEffect::COperatorEffectAbstract	(const T2 condition, const T3 value, bool add_condition) :
	m_condition(condition),
	m_value(value),
	m_add_condition(add_condition)
{
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperatorEffect::add_condition			() const
{
	return				(m_add_condition);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorEffect
