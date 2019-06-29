////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager manager wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_manager_wrapper.h"
#include "script_game_object.h"

CScriptMotivationManagerWrapper::~CScriptMotivationManagerWrapper	()
{
}

void CScriptMotivationManagerWrapper::setup			(_object_type *object)
{
	luabind::call_member<void>						(this,"setup",object);
}

void CScriptMotivationManagerWrapper::setup_static	(CScriptMotivationManager *manager, CScriptGameObject *object)
{
	manager->CScriptMotivationManager::setup		(object);
}

void CScriptMotivationManagerWrapper::update			()
{
	luabind::call_member<void>						(this,"update");
}

void CScriptMotivationManagerWrapper::update_static	(CScriptMotivationManager *manager)
{
	manager->CScriptMotivationManager::update	();
}