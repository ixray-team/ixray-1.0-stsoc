////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_no_items.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object no items property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

class CObjectPropertyEvaluatorNoItems : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
						CObjectPropertyEvaluatorNoItems	(CAI_Stalker *owner);
	virtual _value_type	evaluate						();
};