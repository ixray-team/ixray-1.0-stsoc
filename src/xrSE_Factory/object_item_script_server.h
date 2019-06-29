////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_server.h
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item server script class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"
#include "script_space.h"

class CObjectItemScriptServer : virtual public CObjectItemAbstract {
protected:
	typedef CObjectItemAbstract	inherited;

protected:
	luabind::functor<void>		m_server_creator;

public:
	IC											CObjectItemScriptServer	(
													luabind::functor<void> server_creator, 
													const CLASS_ID &clsid, 
													LPCSTR script_clsid
												);
#ifndef NO_XR_GAME
	virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object		() const;
#endif
	virtual ObjectFactory::SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
};

#include "object_item_script_server_inline.h"