////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "lstate.h"
#include "script_engine.h"
#include "ai_script.h"
#include "ai_space.h"

#include "script_debugger.h"

CScript::CScript(LPCSTR caNamespaceName)
{
	string256			S;
	m_bActive			= false;
	m_script_name		= xr_strdup(caNamespaceName);
	Msg					("* Loading design script %s",caNamespaceName);

	ai().script_engine().add_file(caNamespaceName);
	ai().script_engine().process();

	m_virtual_machine	= lua_newthread(ai().script_engine().lua());
	VERIFY2				(lua(),"Cannot create new Lua thread");
	m_thread_reference	= luaL_ref(ai().script_engine().lua(),LUA_REGISTRYINDEX);
	
#ifdef DEBUG
	if( !CScriptDebugger::GetDebugger()->Active() )
		lua_sethook		(lua(),CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
	else
		lua_sethook		(lua(), CDbgLuaHelper::hookLua,			LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);
#endif

//	sprintf				(S,"function start_thread()\n%s.main()\nend\n",caNamespaceName);
	sprintf				(S,"%s.main()",caNamespaceName);
	if (!ai().script_engine().load_buffer(lua(),S,xr_strlen(S),"@_thread_main"))
		return;

//	lua_call			(lua(),0,0);

//	ai().script_engine().set_current_thread	(m_script_name);
//	luabind::resume_function<void>(lua(),"start_thread");
//	ai().script_engine().set_current_thread	("");

	m_bActive			= true;
}

CScript::~CScript()
{
	luaL_unref				(ai().script_engine().lua(),LUA_REGISTRYINDEX,m_thread_reference);
	xr_delete				(m_script_name);
}

bool CScript::Update()
{
	if (!m_bActive)
		R_ASSERT2		(false,"Cannot resume dead Lua thread!");
	
	ai().script_engine().set_current_thread	(this);
	
	int					l_iErrorCode = lua_resume(lua(),0);
//	luabind::resume<void>(lua());
	if (l_iErrorCode) {
		if (!ai().script_engine().print_output(lua(),m_script_name,l_iErrorCode))
			ai().script_engine().print_error(lua(),l_iErrorCode);
		m_bActive		= false;
	}
	else
		if (!(lua()->ci->state & CI_YIELD)) {
			m_bActive	= false;
			ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished!",m_script_name);
		}
		else {
			if( !CScriptDebugger::GetDebugger()->Active() ) {
				VERIFY		(m_current_stack_level);
				--m_current_stack_level;
			}
			VERIFY2		(!lua_gettop(lua()),"Do not pass any value to coroutine.yield()!");
		}
	
	ai().script_engine().set_current_thread	(0);
	
	return				(m_bActive);
}