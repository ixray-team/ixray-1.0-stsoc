////////////////////////////////////////////////////////////////////////////
//	Module 		: script_property_evaluator.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script property_evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator.h"

class CScriptPropertyEvaluator : public CPropertyEvaluator<CLuaGameObject> {
protected:
	typedef CPropertyEvaluator<CLuaGameObject> inherited;

public:
	IC					CScriptPropertyEvaluator	(CLuaGameObject *object = 0);
	virtual void		reinit						(CLuaGameObject *object);
	virtual	_value_type	evaluate					();
};

#include "script_property_evaluator_inline.h"