////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptCallback::CScriptCallback	() 
{
	init	();
}

IC	void CScriptCallback::init			()
{
	m_lua_function	= 0;
	m_lua_object	= 0;
	m_method_name	= 0;
}

IC	luabind::functor<void> *CScriptCallback::get_function	()
{
	return	(m_lua_function);
}

IC	luabind::object	*CScriptCallback::get_object	()
{
	return	(m_lua_object);
}

IC	shared_str	CScriptCallback::get_method	()
{
	return	(m_method_name);
}

IC	bool CScriptCallback::assigned		() const
{
	return	(m_lua_function || m_lua_object);
}
