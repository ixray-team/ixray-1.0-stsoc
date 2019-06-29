////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptAction::CScriptAction			(CLuaGameObject *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

IC	void CScriptAction::add_condition		(COperatorCondition &condition)
{
	inherited::add_condition(condition);
}

IC	void CScriptAction::remove_condition	(COperatorCondition::_condition_type condition)
{
	inherited::remove_condition(condition);
}

IC	void CScriptAction::add_effect			(COperatorCondition &effect)
{
	inherited::add_effect	(effect);
}

IC	void CScriptAction::remove_effect		(COperatorCondition::_condition_type effect)
{
	inherited::remove_effect(effect);
}