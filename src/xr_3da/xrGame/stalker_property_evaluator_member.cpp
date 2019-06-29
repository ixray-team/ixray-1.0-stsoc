////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_member.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker member property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluator_member.h"

CStalkerPropertyEvaluatorMember::CStalkerPropertyEvaluatorMember	(_value_type *value) :
	m_value			(value)
{
}

CStalkerPropertyEvaluatorMember::_value_type CStalkerPropertyEvaluatorMember::evaluate	()
{
	return			(*m_value);
}
