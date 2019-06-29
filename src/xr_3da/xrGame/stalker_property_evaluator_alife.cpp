////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_alife.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluator_alife.h"

CStalkerPropertyEvaluatorALife::_value_type CStalkerPropertyEvaluatorALife::evaluate	()
{
	return			(!!ai().get_alife());
}
