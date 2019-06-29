////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_alive.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alive property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluator_alive.h"
#include "ai/stalker/ai_stalker.h"

CStalkerPropertyEvaluatorAlive::_value_type CStalkerPropertyEvaluatorAlive::evaluate	()
{
	return			(!!m_object->g_Alive());
}
