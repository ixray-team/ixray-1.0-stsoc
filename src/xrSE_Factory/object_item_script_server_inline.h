////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_server_inline.h
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item server script class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectItemScriptServer::CObjectItemScriptServer	(
	luabind::functor<void>		server_creator, 
	const CLASS_ID				&clsid, 
	LPCSTR						script_clsid
) : 
	inherited					(clsid,script_clsid)
{
	m_server_creator			= server_creator;
}
