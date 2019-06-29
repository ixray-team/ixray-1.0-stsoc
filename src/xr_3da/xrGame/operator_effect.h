////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_effect.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator effect
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_condition.h"

template <
	typename T2,
	typename T3
>
class COperatorEffectAbstract : 
	public COperatorConditionAbstract<T2,T3>
{
private:
	typedef COperatorConditionAbstract<T2,T3> inherited;

protected:
	bool				m_add_condition;

public:
	IC					COperatorEffectAbstract		(const T2 condition, const T3 value, bool add_condition);
	virtual				~COperatorEffectAbstract	();
	IC		bool		add_condition				() const;
};

#include "operator_effect_inline.h"