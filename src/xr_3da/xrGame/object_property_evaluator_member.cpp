////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_member.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object member property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_member.h"

CObjectPropertyEvaluatorMember::CObjectPropertyEvaluatorMember	(CGameObject *item, CAI_Stalker *owner, bool *member, bool equality) :
	inherited		(item,owner),
	m_member		(member),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorMember::_value_type CObjectPropertyEvaluatorMember::evaluate	()
{
	return			(_value_type(*m_member == m_equality));
}
