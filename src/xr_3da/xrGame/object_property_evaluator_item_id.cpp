////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_item_id.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item id property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_item_id.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"

CObjectPropertyEvaluatorItemID::CObjectPropertyEvaluatorItemID(CAI_Stalker *owner)
{
	m_object		= owner;
}

CObjectPropertyEvaluatorItemID::_value_type CObjectPropertyEvaluatorItemID::evaluate	()
{
	return			(!m_object->inventory().ActiveItem());
}
