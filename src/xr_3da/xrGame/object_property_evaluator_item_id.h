////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_item_id.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item id property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

class CObjectPropertyEvaluatorItemID : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
						CObjectPropertyEvaluatorItemID	(CAI_Stalker *owner);
	virtual _value_type	evaluate						();
};