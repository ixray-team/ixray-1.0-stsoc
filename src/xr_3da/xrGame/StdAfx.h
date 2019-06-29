#pragma once

#pragma warning(disable:4995)
#include "../stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4505)

// this include MUST be here, since smart_cast is used >1800 times in the project
#include "smart_cast.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	((ltx->line_exist(section,name)) ? (ltx->method(section,name)) : (default_value))


#if XRAY_EXCEPTIONS
IC	xr_string	string2xr_string(LPCSTR s) {return s ? s : "";}
IC	void		throw_and_log(const xr_string &s) {Msg("! %s",s.c_str()); throw s;}
#	define	THROW(xpr)				if (!(xpr)) {throw_and_log (__FILE__LINE__" Expression \""#xpr"\"");}
#	define	THROW2(xpr,msg0)		if (!(xpr)) {throw_and_log (xr_string(__FILE__LINE__).append(" Expression \"").append(#xpr).append(string2xr_string(msg0)));}
#	define	THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw_and_log (xr_string(__FILE__LINE__).append(" Expression \"").append(#xpr).append(string2xr_string(msg0)).append(", ").append(string2xr_string(msg1)));}
#else
#	define	THROW					VERIFY
#	define	THROW2					VERIFY2
#	define	THROW3					VERIFY3
#endif

#include "../gamefont.h"
#include "../xr_object.h"
#include "../igame_level.h"

//#define MASTER_GOLD
