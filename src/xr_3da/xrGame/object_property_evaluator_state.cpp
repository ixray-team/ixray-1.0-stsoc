////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_state.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_state.h"
#include "weapon.h"

CObjectPropertyEvaluatorState::CObjectPropertyEvaluatorState	(CWeapon *item, CAI_Stalker *owner, u32 state, bool equality) :
	inherited		(item,owner),
	m_state			(state),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorState::_value_type CObjectPropertyEvaluatorState::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type((m_item->STATE == m_state) == m_equality));
}
