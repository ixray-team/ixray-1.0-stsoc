////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner.h"

class CScriptActionPlanner : public CActionPlanner<CLuaGameObject> {
protected:
	typedef CActionPlanner<CLuaGameObject> inherited;

public:
	virtual	void			reinit					(CLuaGameObject *object, bool clear_all = false);
	virtual	void			update					(u32 time_delta);
	IC		void			set_target_state		(CState &state);
	IC		void			add_evaluator			(_condition_type condition_id, CConditionEvaluator *evaluator);
	IC		void			remove_evaluator		(_condition_type condition_id);
	IC		const CConditionEvaluator	*evaluator	(_condition_type condition_id) const;
	IC		void			add_operator			(_edge_type	operator_id, COperator *_operator);
	IC		void			remove_operator			(_edge_type	operator_id);
	IC		COperator		*action					(_action_id_type action_id);
	IC		_action_id_type	current_action_id		();
	IC		COperator		*current_action			();
	IC		bool			initialized				();
};

#include "script_action_planner_inline.h"