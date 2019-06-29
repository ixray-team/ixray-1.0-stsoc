////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_missile.cpp
//	Created 	: 18.03.2004
//  Modified 	: 18.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object missile property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_missile.h"
#include "missile.h"

CObjectPropertyEvaluatorMissile::CObjectPropertyEvaluatorMissile	(CMissile *item, CAI_Stalker *owner, u32 state, bool equality) :
	inherited		(item,owner),
	m_state			(state),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorMissile::_value_type CObjectPropertyEvaluatorMissile::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type((m_item->State() == m_state) == m_equality));
}
