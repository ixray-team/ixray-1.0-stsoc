////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_state_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CScriptWorldState::COperatorCondition	*CScriptWorldState::property(COperatorCondition::_condition_type condition) const
{
	xr_vector<COperatorCondition>::const_iterator	I = std::lower_bound(conditions().begin(),conditions().end(),COperatorCondition(condition,COperatorCondition::_value_type(0)));
	if ((I != conditions().end()) && ((*I).condition() == condition))
		return				(&*I);
	else
		return				(0);
}

IC	void CScriptWorldState::remove_condition	(COperatorCondition::_condition_type condition)
{
	inherited::remove_condition	(condition);
}

IC	bool CScriptWorldState::includes			(inherited &condition) const
{
	return					(inherited::includes(condition));
}

IC	void CScriptWorldState::clear				()
{
	inherited::clear		();
}
