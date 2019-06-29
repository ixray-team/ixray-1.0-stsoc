////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_missile.h
//	Created 	: 18.03.2004
//  Modified 	: 18.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object missile property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CMissile;

class CObjectPropertyEvaluatorMissile : public CObjectPropertyEvaluatorBase<CMissile> {
protected:
	typedef CObjectPropertyEvaluatorBase<CMissile> inherited;
	u32						m_state;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorMissile	(CMissile *item, CAI_Stalker *owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};