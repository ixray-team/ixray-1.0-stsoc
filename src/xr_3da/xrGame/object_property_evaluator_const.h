////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_const.h
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object const property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_property_evaluator_base.h"

class CObjectPropertyEvaluatorConst : public CSObjectPropertyEvaluatorBase {
protected:
	typedef CSObjectPropertyEvaluatorBase	inherited;
	_value_type			m_value;

public:
						CObjectPropertyEvaluatorConst	(CGameObject *item, CAI_Stalker *owner, _value_type value);
	virtual _value_type	evaluate						();
};