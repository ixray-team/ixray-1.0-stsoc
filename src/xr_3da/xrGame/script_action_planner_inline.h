////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptActionPlanner::set_target_state		(CState &state)
{
	inherited::set_target_state(state);
}

IC	void CScriptActionPlanner::add_evaluator		(_condition_type condition_id, CConditionEvaluator *evaluator)
{
	inherited::add_evaluator(condition_id,evaluator);
}

IC	void CScriptActionPlanner::remove_evaluator		(_condition_type condition_id)
{
	inherited::remove_evaluator(condition_id);
}

IC	const CScriptActionPlanner::CConditionEvaluator	*CScriptActionPlanner::evaluator	(_condition_type condition_id) const
{
	return				(inherited::evaluator(condition_id));
}

IC	void CScriptActionPlanner::add_operator			(_edge_type	operator_id, COperator *_operator)
{
	inherited::add_operator(operator_id,_operator);
}

IC	void CScriptActionPlanner::remove_operator		(_edge_type	operator_id)
{
	inherited::remove_operator(operator_id);
}

IC	CScriptActionPlanner::COperator	*CScriptActionPlanner::action	(_action_id_type action_id)
{
	return			(&inherited::action(action_id));
}

IC	CScriptActionPlanner::_action_id_type CScriptActionPlanner::current_action_id	()
{
	return			(inherited::current_action_id());
}

IC	CScriptActionPlanner::COperator	*CScriptActionPlanner::current_action	()
{
	return			(&inherited::current_action());
}

IC	bool CScriptActionPlanner::initialized			()
{
	return			(inherited::initialized());
}