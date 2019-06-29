//////////////////////////////////////////////////////////////////////
// inventory_owner_scripts.cpp:	функции для работы со скриптами	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryOwner.h"
#include "script_space.h"
#include "script_callback.h"

void CInventoryOwner::set_pda_callback	(const luabind::object &lua_object, LPCSTR method)
{
	m_pPdaCallback->set(lua_object, method);
}
void CInventoryOwner::set_pda_callback	(const luabind::functor<void> &lua_function)
{
	m_pPdaCallback->set(lua_function);
}
void CInventoryOwner::set_info_callback	(const luabind::object &lua_object, LPCSTR method)
{
	m_pInfoCallback->set(lua_object, method);
}
void CInventoryOwner::set_info_callback	(const luabind::functor<void> &lua_function)
{
	m_pInfoCallback->set(lua_function);
}
void CInventoryOwner::clear_pda_callback ()
{
	m_pPdaCallback->clear();
}

void CInventoryOwner::clear_info_callback ()
{
	m_pInfoCallback->clear();
}