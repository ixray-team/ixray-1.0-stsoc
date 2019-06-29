////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluator_member.h
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker member property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

class CStalkerPropertyEvaluatorMember : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;
	_value_type			*m_value;

public:
						CStalkerPropertyEvaluatorMember	(_value_type *value);
	virtual _value_type	evaluate						();
};