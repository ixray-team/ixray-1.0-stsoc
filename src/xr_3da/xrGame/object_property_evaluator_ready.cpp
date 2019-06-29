////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_ready.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object ready property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_ready.h"
#include "weapon.h"

CObjectPropertyEvaluatorReady::CObjectPropertyEvaluatorReady(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

CObjectPropertyEvaluatorReady::_value_type CObjectPropertyEvaluatorReady::evaluate	()
{
	if (!m_ammo_type)
		return		(_value_type(!m_item->IsMisfire() && m_item->GetAmmoElapsed()));
	else
		return		(_value_type(false));
}
