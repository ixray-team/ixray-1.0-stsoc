////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_empty.h
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object empty property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CWeapon;

class CObjectPropertyEvaluatorEmpty : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
							CObjectPropertyEvaluatorEmpty	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};