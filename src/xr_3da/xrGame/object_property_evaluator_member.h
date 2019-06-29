////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_member.h
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object member property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CObjectPropertyEvaluatorMember : public CSObjectPropertyEvaluatorBase {
protected:
	typedef CSObjectPropertyEvaluatorBase inherited;
	bool					*m_member;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorMember	(CGameObject *item, CAI_Stalker *owner, bool *member, bool equality = true);
	virtual _value_type		evaluate						();
};