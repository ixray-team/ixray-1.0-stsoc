////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "script_action.h"

void CScriptAction::reinit			(CLuaGameObject *object, bool clear_all)
{
	inherited::reinit		(object,clear_all);
}

void CScriptAction::initialize		()
{
	inherited::initialize	();
}

void CScriptAction::execute			()
{
	inherited::execute		();
}

void CScriptAction::finalize		()
{
	inherited::finalize		();
}

CScriptAction::_edge_value_type	CScriptAction::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return					(inherited::weight(condition0,condition1));
}