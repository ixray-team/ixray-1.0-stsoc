////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_base.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CAI_Stalker;

template <typename _item_type>
class CObjectPropertyEvaluatorBase : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;
	_item_type		*m_item;

public:
	IC					CObjectPropertyEvaluatorBase(_item_type *item, CAI_Stalker *owner);
};

typedef CObjectPropertyEvaluatorBase<CGameObject> CSObjectPropertyEvaluatorBase;

#include "object_property_evaluator_base_inline.h"