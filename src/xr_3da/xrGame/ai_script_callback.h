#pragma once
#include "script_space.h"

class CScriptCallback {

	luabind::functor<void>		*m_lua_function;
	luabind::object				*m_lua_object;
	ref_str						m_method_name;

public:
				CScriptCallback		() 
	{	
		m_lua_function	= 0;
		m_lua_object	= 0;
	}
	virtual		~CScriptCallback	() {
		xr_delete(m_lua_function);
		xr_delete(m_lua_object);
	}

	void		set					(const luabind::functor<void> &lua_function)	{
		
		xr_delete		(m_lua_function);
		m_lua_function	= xr_new<luabind::functor<void> >(lua_function);

	}

	void		set					(const luabind::object &lua_object, LPCSTR method)	{
		xr_delete		(m_lua_object);
		m_lua_object	= xr_new<luabind::object>(lua_object);
		m_method_name	= method;
	}
	
	void		clear				() {
		xr_delete		(m_lua_function);
		xr_delete		(m_lua_object);
	}

	void		clear				(bool member) {
		(member) ? xr_delete(m_lua_object) : xr_delete(m_lua_function);
	}

	luabind::functor<void>	*get_function	() {return m_lua_function;}
	luabind::object			*get_object		() {return m_lua_object;}
	ref_str					get_method		() {return m_method_name;}


	void		callback			() {
		if (m_lua_function)	(*m_lua_function)();
		if (m_lua_object)	luabind::call_member<void>(*m_lua_object,*m_method_name);
	}

	IC		bool		assigned	() const
	{
		return				(m_lua_function || m_lua_object);
	}

};

#define SCRIPT_CALLBACK_EXECUTE_0(callback_class) {																							\
	(callback_class).callback();																												\
}

#define SCRIPT_CALLBACK_EXECUTE_1(callback_class,p1) {																						\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1);																\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1);			\
}

#define SCRIPT_CALLBACK_EXECUTE_2(callback_class,p1,p2) {																					\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2);		\
}

#define SCRIPT_CALLBACK_EXECUTE_3(callback_class,p1,p2,p3) {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3);	\
}

#define SCRIPT_CALLBACK_EXECUTE_4(callback_class,p1,p2,p3,p4) {																					\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4);		\
}

#define SCRIPT_CALLBACK_EXECUTE_5(callback_class,p1,p2,p3,p4,p5) {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5);	\
}

#define SCRIPT_CALLBACK_EXECUTE_6(callback_class,p1,p2,p3,p4,p5,p6) {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6);	\
}

#define SCRIPT_CALLBACK_EXECUTE_7(callback_class,p1,p2,p3,p4,p5,p6,p7) {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6,p7);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6,p7);	\
}

#define SCRIPT_CALLBACK_EXECUTE_8(callback_class,p1,p2,p3,p4,p5,p6,p7,p8) {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6,p7,p8);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6,p7,p8);	\
}
