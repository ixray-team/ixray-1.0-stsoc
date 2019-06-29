////////////////////////////////////////////////////////////////////////////
//	Module 		: action_management_config.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action management configuration file
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef DEBUG
#	define LOG_ACTION
	extern BOOL g_use_scripts_in_goap;
#	define THROW_EXCEPTION_IF_NO_SCRIPTS_IN_GOAP \
	do {\
		if (!g_use_scripts_in_goap)\
			throw "no scripts exception";\
	} while (0)
#else
#	define THROW_EXCEPTION_IF_NO_SCRIPTS_IN_GOAP
#endif
