////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_client.h
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client script class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"
#include "script_space.h"

class CObjectItemScriptClient : virtual public CObjectItemAbstract {
protected:
	typedef CObjectItemAbstract	inherited;

protected:
	luabind::functor<void>		m_client_creator;

public:
	IC											CObjectItemScriptClient	(
													luabind::functor<void> client_creator, 
													const CLASS_ID &clsid, 
													LPCSTR script_clsid
												);
	virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object		() const;
	virtual ObjectFactory::SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
};

#include "object_item_script_client_inline.h"