////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback.cpp
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_callback.h"
#include "script_space.h"

CScriptCallback::~CScriptCallback	()
{
	xr_delete(m_lua_function);
	xr_delete(m_lua_object);
}

CScriptCallback::CScriptCallback	(const CScriptCallback &callback)
{
	init				();

	if (callback.m_lua_function)
		m_lua_function	= xr_new<luabind::functor<void> >(*callback.m_lua_function);

	if (callback.m_lua_object)
		m_lua_object	= xr_new<luabind::object>(*callback.m_lua_object);

	m_method_name		= callback.m_method_name;
}

void CScriptCallback::callback		()
{
	if (m_lua_function)
		(*m_lua_function)			();
	if (m_lua_object)
		luabind::call_member<void>	(*m_lua_object,*m_method_name);
}

void CScriptCallback::set			(const luabind::functor<void> &lua_function)
{
	
	xr_delete		(m_lua_function);
	m_lua_function	= xr_new<luabind::functor<void> >(lua_function);

}

void CScriptCallback::set			(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete		(m_lua_object);
	m_lua_object	= xr_new<luabind::object>(lua_object);
	m_method_name	= method;
}

void CScriptCallback::clear			()
{
	try {
		xr_delete		(m_lua_function);
		xr_delete		(m_lua_object);
		m_method_name	= 0;
	}
	catch(...) {
	}

#ifdef DEBUG
	init				();
#endif
}

void CScriptCallback::clear			(bool member)
{
	(member) ? xr_delete(m_lua_object) : xr_delete(m_lua_function);
}

