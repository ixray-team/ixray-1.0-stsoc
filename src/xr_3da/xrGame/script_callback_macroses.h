////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_macroses.h
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callback macroses
////////////////////////////////////////////////////////////////////////////

#pragma once

#define SCRIPT_CALLBACK_EXECUTE_0(callback_class) try {																							\
	(callback_class).callback();																												\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_1(callback_class,p1) try {																						\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1);																\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1);			\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_2(callback_class,p1,p2) try {																					\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2);		\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_3(callback_class,p1,p2,p3) try {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3);	\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_4(callback_class,p1,p2,p3,p4) try {																					\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4);		\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_5(callback_class,p1,p2,p3,p4,p5) try {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5);	\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_6(callback_class,p1,p2,p3,p4,p5,p6) try {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6);	\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_7(callback_class,p1,p2,p3,p4,p5,p6,p7) try {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6,p7);															\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6,p7);	\
}catch(...){(callback_class).clear();}

#define SCRIPT_CALLBACK_EXECUTE_8(callback_class,p1,p2,p3,p4,p5,p6,p7,p8) try {																				\
	if ((callback_class).get_function())	(*((callback_class).get_function()))(p1,p2,p3,p4,p5,p6,p7,p8);														\
	if ((callback_class).get_object())		luabind::call_member<void>(*((callback_class).get_object()),*((callback_class).get_method()), p1,p2,p3,p4,p5,p6,p7,p8);	\
}catch(...){(callback_class).clear();}
