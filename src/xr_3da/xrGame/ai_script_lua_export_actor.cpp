////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extport_actor.cpp
//	Created 	: 01.12.2003
//  Modified 	: 01.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Actor export to script
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_classes.h"
#include "actor.h"

CLuaGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return(xr_new<CLuaGameObject>(dynamic_cast<CGameObject*>(l_tpActor)));
	else
		return(0);
}