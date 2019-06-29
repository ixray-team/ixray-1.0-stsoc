////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback.h
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space_forward.h"

class CScriptCallback {
private:
	luabind::functor<void>			*m_lua_function;
	luabind::object					*m_lua_object;
	shared_str						m_method_name;

public:
	IC								CScriptCallback		();
									CScriptCallback		(const CScriptCallback &callback);
	virtual							~CScriptCallback	();
			void					set					(const luabind::functor<void> &lua_function);
			void					set					(const luabind::object &lua_object, LPCSTR method);
	IC		void					init				();
			void					clear				();
			void					clear				(bool member);
	IC		luabind::functor<void>	*get_function		();
	IC		luabind::object			*get_object			();
	IC		shared_str				get_method			();
			void					callback			();
	IC		bool					assigned			() const;
};

#include "script_callback_inline.h"
#include "script_callback_macroses.h"