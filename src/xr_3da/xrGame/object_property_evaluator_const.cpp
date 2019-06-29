////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_const.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object const property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_const.h"

CObjectPropertyEvaluatorConst::CObjectPropertyEvaluatorConst	(CGameObject *item, CAI_Stalker *owner, _value_type value) :
	inherited		(item,owner),
	m_value			(value)
{
}

CObjectPropertyEvaluatorConst::_value_type CObjectPropertyEvaluatorConst::evaluate	()
{
	return			(m_value);
}
