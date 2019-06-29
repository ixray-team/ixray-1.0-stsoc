////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_ready.h
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object ready property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CWeapon;

class CObjectPropertyEvaluatorReady : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
							CObjectPropertyEvaluatorReady	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};