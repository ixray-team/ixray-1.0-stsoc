////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_manager_wrapper.cpp
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager action manager wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_action_manager_wrapper.h"
#include "script_game_object.h"

CScriptMotivationActionManagerWrapper::~CScriptMotivationActionManagerWrapper	()
{
}

void CScriptMotivationActionManagerWrapper::setup			(_object_type *object)
{
	luabind::call_member<void>						(this,"setup",object);
}

void CScriptMotivationActionManagerWrapper::setup_static	(CScriptMotivationActionManager *manager, CScriptGameObject *object)
{
	manager->CScriptMotivationActionManager::setup	(object);
}

void CScriptMotivationActionManagerWrapper::update			()
{
	luabind::call_member<void>						(this,"update");
}

void CScriptMotivationActionManagerWrapper::update_static	(CScriptMotivationActionManager *manager)
{
	manager->CScriptMotivationActionManager::update	();
}