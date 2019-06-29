////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_const.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker const property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluator_const.h"

CStalkerPropertyEvaluatorConst::CStalkerPropertyEvaluatorConst	(_value_type value) :
	m_value			(value)
{
}

CStalkerPropertyEvaluatorConst::_value_type CStalkerPropertyEvaluatorConst::evaluate	()
{
	return			(m_value);
}
