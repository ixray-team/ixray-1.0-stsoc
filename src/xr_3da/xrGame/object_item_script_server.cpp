////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_server.cpp
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item server script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_item_script_server.h"
#include "object_factory.h"

#ifndef NO_XR_GAME
ObjectFactory::CLIENT_BASE_CLASS *CObjectItemScriptServer::client_object	() const
{
	NODEFAULT;
#ifdef DEBUG
	return						(0);
#endif
}
#endif

ObjectFactory::SERVER_BASE_CLASS *CObjectItemScriptServer::server_object	(LPCSTR section) const
{
	object_factory().set_instance((ObjectFactory::SERVER_SCRIPT_BASE_CLASS*)0);
	m_server_creator			(const_cast<CObjectFactory*>(&object_factory()),section);
	return						(object_factory().server_instance());
}
