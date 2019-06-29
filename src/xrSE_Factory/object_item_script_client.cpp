////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_client.cpp
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_item_script_client.h"
#include "object_factory.h"
#include "attachable_item.h"

ObjectFactory::CLIENT_BASE_CLASS *CObjectItemScriptClient::client_object	() const
{
	object_factory().set_instance((ObjectFactory::CLIENT_SCRIPT_BASE_CLASS*)0);
	m_client_creator			(const_cast<CObjectFactory*>(&object_factory()));
	return						(object_factory().client_instance());
}

ObjectFactory::SERVER_BASE_CLASS *CObjectItemScriptClient::server_object	(LPCSTR section) const
{
	NODEFAULT;
#ifdef DEBUG
	return						(0);
#endif
}
