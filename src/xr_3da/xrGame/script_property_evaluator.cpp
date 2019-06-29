////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property_evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_property_evaluator.h"

void CScriptPropertyEvaluator::reinit	(CLuaGameObject *object)
{
	inherited::reinit	(object);
}

CScriptPropertyEvaluator::_value_type CScriptPropertyEvaluator::evaluate	()
{
	return				(inherited::evaluate());
}
