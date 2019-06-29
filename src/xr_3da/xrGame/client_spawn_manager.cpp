////////////////////////////////////////////////////////////////////////////
//	Module 		: client_spawn_manager.cpp
//	Created 	: 08.10.2004
//  Modified 	: 08.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Seniority hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h" 
#include "client_spawn_manager.h" 
#include "ai_space.h" 
#include "script_engine.h" 
#include "level.h"
#include "gameobject.h"
#include "script_space.h"
#include "script_game_object.h"

CClientSpawnManager::~CClientSpawnManager	()
{
}

void CClientSpawnManager::add		(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const luabind::functor<void> &functor, const luabind::object &object)
{
	CSpawnCallback					callback;
	callback.m_callback.set			(functor,object);
	add								(requesting_id,requested_id,callback);
}

void CClientSpawnManager::add		(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const luabind::functor<void> &lua_function)
{
	CSpawnCallback					callback;
	callback.m_callback.set			(lua_function);
	add								(requesting_id,requested_id,callback);
}

void CClientSpawnManager::add				(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const CALLBACK_TYPE &object_callback)
{
	CSpawnCallback					callback;
	callback.m_object_callback		= object_callback;
	add								(requesting_id,requested_id,callback);
}

void CClientSpawnManager::add				(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, CSpawnCallback &spawn_callback)
{
	CObject							*object = Level().Objects.net_Find(requesting_id);
	if (object) {
		callback					(spawn_callback,object);
		return;
	}

	REQUEST_REGISTRY::iterator		I = m_registry.find(requesting_id);
	if (I == m_registry.end()) {
		REQUESTED_REGISTRY			registry;
		registry.insert				(std::make_pair(requested_id,spawn_callback));
		m_registry.insert			(std::make_pair(requesting_id,registry));
		return;
	}
	
	REQUESTED_REGISTRY::iterator	J = (*I).second.find(requested_id);
	if (J == (*I).second.end())
		(*I).second.insert			(std::make_pair(requested_id,spawn_callback));
	else
		merge_spawn_callbacks		(spawn_callback,(*J).second);
}

void CClientSpawnManager::remove			(REQUESTED_REGISTRY &registry, ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, bool no_warning)
{
	REQUESTED_REGISTRY::iterator	I = registry.find(requested_id);
	if (I != registry.end())
		registry.erase				(I);
	else
		ai().script_engine().script_log	(eLuaMessageTypeError,"There is no spawn callback on object with id %d from object with id %d!",requesting_id,requested_id);
}

void CClientSpawnManager::remove			(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id)
{
	REQUEST_REGISTRY::iterator		I = m_registry.find(requesting_id);
	if (I != m_registry.end())
		remove						((*I).second,requesting_id,requested_id);
	else
		ai().script_engine().script_log	(eLuaMessageTypeError,"There is no spawn callback on object with id %d from object with id %d!",requesting_id,requested_id);
}

void CClientSpawnManager::clear				(ALife::_OBJECT_ID requested_id)
{
	REQUEST_REGISTRY::iterator		I = m_registry.begin();
	REQUEST_REGISTRY::iterator		E = m_registry.end();
	for ( ; I != E; ++I)
		remove						((*I).second,(*I).first,requested_id,true);
}

void CClientSpawnManager::callback			(CSpawnCallback &spawn_callback, CObject *object)
{
	if (spawn_callback.m_object_callback)
		spawn_callback.m_object_callback	(object);
	CGameObject						*game_object = smart_cast<CGameObject*>(object);
	CScriptGameObject				*script_game_object = !game_object ? 0 : game_object->lua_game_object();
	(spawn_callback.m_callback)	(object->ID(),script_game_object);
}

void CClientSpawnManager::callback			(CObject *object)
{
	REQUEST_REGISTRY::iterator		I = m_registry.find(object->ID());
	if (I == m_registry.end())
		return;

	REQUESTED_REGISTRY::iterator	i = (*I).second.begin();
	REQUESTED_REGISTRY::iterator	e = (*I).second.end();
	for ( ; i != e; ++i)
		callback					((*i).second,object);

	(*I).second.clear				();
}

void CClientSpawnManager::merge_spawn_callbacks	(CSpawnCallback &new_callback, CSpawnCallback &old_callback)
{
	old_callback.m_object_callback	= new_callback.m_object_callback;
	old_callback.m_callback			= new_callback.m_callback;
}

const CClientSpawnManager::CSpawnCallback *CClientSpawnManager::callback(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id) const
{
	REQUEST_REGISTRY::const_iterator	I = m_registry.find(requested_id);
	if (I == m_registry.end())
		return						(0);

	REQUESTED_REGISTRY::const_iterator	i = (*I).second.find(requesting_id);
	if (i == (*I).second.end())
		return						(0);

	return							(&(*i).second);
}
