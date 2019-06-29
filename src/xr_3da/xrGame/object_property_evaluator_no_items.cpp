////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_no_items.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object no items property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_no_items.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectPropertyEvaluatorNoItems::CObjectPropertyEvaluatorNoItems(CAI_Stalker *owner)
{
	m_object		= owner;
}

CObjectPropertyEvaluatorNoItems::_value_type CObjectPropertyEvaluatorNoItems::evaluate	()
{
	return			(!m_object->inventory().ActiveItem());
}