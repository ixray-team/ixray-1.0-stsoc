////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_state.h
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CWeapon;

class CObjectPropertyEvaluatorState : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_state;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorState	(CWeapon *item, CAI_Stalker *owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};