////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_empty.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object empty property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_empty.h"
#include "weapon.h"

CObjectPropertyEvaluatorEmpty::CObjectPropertyEvaluatorEmpty(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

CObjectPropertyEvaluatorEmpty::_value_type CObjectPropertyEvaluatorEmpty::evaluate	()
{
	if (!m_ammo_type)
		return		(_value_type(!m_item->GetAmmoElapsed()));
	else
		return		(_value_type(false));
}
