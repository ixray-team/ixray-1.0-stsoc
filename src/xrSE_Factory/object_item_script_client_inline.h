////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_client_inline.h
//	Created 	: 05.07.2004
//  Modified 	: 05.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client script class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectItemScriptClient::CObjectItemScriptClient	(
	luabind::functor<void>		client_creator, 
	const CLASS_ID				&clsid, 
	LPCSTR						script_clsid
) : 
	inherited					(clsid,script_clsid)
{
	m_client_creator			= client_creator;
}
