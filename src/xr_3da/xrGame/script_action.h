////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CScriptAction : public CActionBase<CLuaGameObject> {
protected:
	typedef CActionBase<CLuaGameObject> inherited;

public:
	IC							CScriptAction	(CLuaGameObject *object = 0, LPCSTR action_name = "");
	IC		void				add_condition	(COperatorCondition &condition);
	IC		void				remove_condition(COperatorCondition::_condition_type condition);
	IC		void				add_effect		(COperatorCondition &effect);
	IC		void				remove_effect	(COperatorCondition::_condition_type effect);
	virtual	void				reinit			(CLuaGameObject *object, bool clear_all);
	virtual void				initialize		();
	virtual void				execute			();
	virtual void				finalize		();
	virtual _edge_value_type	weight			(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

#include "script_action_inline.h"