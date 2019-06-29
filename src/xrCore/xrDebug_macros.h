#ifndef xrDebug_macrosH
#define xrDebug_macrosH
#pragma once

//.#ifndef __BORLANDC__
	// ---==( Extended Debugging Support (R) )==---
#ifndef __BORLANDC__
#	define DEBUG_INFO					__FILE__,__LINE__,__FUNCTION__
#else
#	define DEBUG_INFO					__FILE__,__LINE__,__FILE__
#endif

#	define R_ASSERT(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT2(expr,e2)			do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT3(expr,e2,e3)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT4(expr,e2,e3)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,e4,DEBUG_INFO,ignore_always);} while(0)
#	define R_CHK(expr)					do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO,ignore_always);} while(0)
#	define R_CHK2(expr,e2)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#	define FATAL(description)			Debug.fatal(DEBUG_INFO,description)

#	ifdef VERIFY
#		undef VERIFY
#	endif // VERIFY

#	ifdef DEBUG
#		define NODEFAULT				FATAL("nodefault reached")
#		define VERIFY(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY2(expr, e2)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY3(expr, e2, e3)	do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY4(expr, e2, e3, e4)do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,e4,DEBUG_INFO,ignore_always);} while(0)
#		define CHK_DX(expr)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO,ignore_always);} while(0)
#	else
#		ifdef __BORLANDC__
#			define NODEFAULT
#		else
#			define NODEFAULT __assume(0)
#		endif
#		define VERIFY(expr)
#		define VERIFY2(expr, e2)
#		define VERIFY3(expr, e2, e3)
#		define VERIFY4(expr, e2, e3, e4)
#		define CHK_DX(a) a
#	endif
/*
#else // __BORLANDC__
	// ---==( Extended Debugging Support (R) )==---
#	define R_ASSERT(expr) if (!(expr)) ::Debug.fail(#expr,__FILE__, __LINE__)
#	define R_ASSERT2(expr,e2) if (!(expr)) ::Debug.fail(#expr,e2,__FILE__, __LINE__)
#	define R_ASSERT3(expr,e2,e3) if (!(expr)) ::Debug.fail(#expr,e2,e3,__FILE__, __LINE__)
#	define R_CHK(expr) { HRESULT hr = expr; if (FAILED(hr)) ::Debug.error(hr,#expr,__FILE__, __LINE__); }
#	define FATAL(description)			Debug.fatal("%s",description)

#	ifdef DEBUG
#		define	NODEFAULT Debug.fatal("nodefault: reached")
#		define VERIFY(expr) if (!(expr)) ::Debug.fail(#expr,__FILE__, __LINE__)
#		define VERIFY2(expr, e2) if (!(expr)) ::Debug.fail(#expr,e2,__FILE__, __LINE__)
#		define VERIFY3(expr, e2, e3) if (!(expr)) ::Debug.fail(#expr,e2,e3,__FILE__, __LINE__)
#		define CHK_DX(expr) { HRESULT hr = expr; if (FAILED(hr)) ::Debug.error(hr,#expr,__FILE__, __LINE__); }
#	else
#		ifdef __BORLANDC__
#			define NODEFAULT
#		else
#			define NODEFAULT __assume(0)
#		endif
#		define VERIFY(expr)
#		define VERIFY2(expr, e2)
#		define VERIFY3(expr, e2, e3)
#		define CHK_DX(a) a
#	endif
#endif // __BORLANDC__
*/
//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
//---------------------------------------------------------------------------------------------
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  TODO :   " #x "\n" \
	" -------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  FIXME :  " #x "\n" \
	" -------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 

//--------- static assertion
template<bool>	struct CompileTimeError;
template<>		struct CompileTimeError<true>	{};
#define STATIC_CHECK(expr, msg) \
{ \
	CompileTimeError<((expr) != 0)> ERROR_##msg; \
	(void)ERROR_##msg; \
}
#endif