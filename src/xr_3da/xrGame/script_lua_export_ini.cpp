////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lua_export_ini.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export ini file
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "script_engine.h"
#include "script_ini_file.h"

using namespace luabind;

void CScriptEngine::export_ini()
{
	module(lua())
	[
		class_<xr_token>("token")
			.def(					constructor<>())
			.def_readwrite("name",	&xr_token::name)
			.def_readwrite("id",	&xr_token::id),

		class_<CScriptTokenList>("token_list")
			.def(					constructor<>())
			.def("add",				CScriptTokenList::add)
			.def("remove",			CScriptTokenList::remove)
			.def("clear",			CScriptTokenList::clear)
			.def("id",				CScriptTokenList::id)
			.def("name",			CScriptTokenList::name),

		class_<CScriptIniFile>("ini_file")
			.def(					constructor<LPCSTR>())
			.def("section_exist",	&CScriptIniFile::section_exist	)
			.def("line_exist",		&CScriptIniFile::line_exist		)
			.def("r_clsid",			&CScriptIniFile::r_clsid		)
			.def("r_bool",			&CScriptIniFile::r_bool			)
			.def("r_token",			&CScriptIniFile::r_token		)
			.def("r_string",		(LPCSTR (CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_string))
			.def("r_u32",			(u32	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_u32))
			.def("r_s32",			(int	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_s32))
			.def("r_float",			(float	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_float))
			.def("r_vector",		(Fvector(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_fvector3))
			.def("line_count",		(u32	(CScriptIniFile::*)	(LPCSTR))		 (CScriptIniFile::line_count))
	];
}

