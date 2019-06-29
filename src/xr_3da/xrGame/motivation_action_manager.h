////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_manager.h"
#include "action_planner.h"
#include "action_planner_action.h"
#include "motivation.h"
#include "motivation_action.h"
#include "script_export_space.h"

class CScriptGameObject;

template <
	typename _object_type,
	template <typename _object_type> class _motivation_type = CMotivation,
	template <typename _object_type> class _motivation_action_type = CMotivationAction
>
class CMotivationActionManager :
	public CMotivationManager<_object_type,_motivation_type>,
	public CActionPlanner<_object_type>
{
protected:
	typedef CMotivationManager<_object_type,_motivation_type>	CSMotivationManager;
	typedef CActionPlanner<_object_type>						CSActionPlanner;
	typedef CMotivationAction<_object_type>						CSMotivationAction;

	using CSActionPlanner::add_condition;

protected:
	IC		void			add_condition				(CWorldState &goal, _condition_type condition_id, _value_type value);

public:
	IC						CMotivationActionManager	();
	virtual					~CMotivationActionManager	();
	virtual void			setup						(_object_type *object);
	virtual void			update						();
	virtual void			clear						();
	IC		void			add_action					(const _edge_type &, CScriptActionPlannerAction *);
	IC		void			clear_motivations			();
	IC		void			clear_actions				();
	IC		_object_type	*object						() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
typedef CMotivationActionManager<CScriptGameObject> CScriptMotivationActionManager;
add_to_type_list(CScriptMotivationActionManager)
#undef script_type_list
#define script_type_list save_type_list(CScriptMotivationActionManager)

#include "motivation_action_manager_inline.h"