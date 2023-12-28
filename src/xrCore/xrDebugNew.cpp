#include "stdafx.h"
#pragma hdrstop

#include "xrdebug.h"

#include "dxerr.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <direct.h>
#pragma warning(pop)

extern bool shared_str_initialized;

#ifdef __BORLANDC__
    #	include "d3d9.h"
    #	include "d3dx9.h"
    #	include "D3DX_Wrapper.h"
    #	pragma comment(lib,"EToolsB.lib")
    #	define DEBUG_INVOKE	DebugBreak()
        static BOOL			bException	= TRUE;
#else
    #	define DEBUG_INVOKE	__asm int 3
        static BOOL			bException	= FALSE;

    #	define USE_OWN_ERROR_MESSAGE_WINDOW
#endif

#ifndef _M_AMD64
#	ifndef __BORLANDC__
#		pragma comment(lib,"dxerr.lib")
#	endif
#endif

#include <dbghelp.h>						// MiniDump flags

#include <new.h>							// for _set_new_mode
#include <signal.h>							// for signals

#ifndef DEBUG
#	define USE_OWN_MINI_DUMP
#endif // DEBUG

XRCORE_API	xrDebug		Debug;

static bool	error_after_dialog = false;

extern void copy_to_clipboard	(const char *string);

void copy_to_clipboard	(const char *string)
{
	if (IsDebuggerPresent())
		return;

	if (!OpenClipboard(0))
		return;

	HGLOBAL				handle = GlobalAlloc(GHND | GMEM_DDESHARE,(strlen(string) + 1)*sizeof(char));
	if (!handle) {
		CloseClipboard	();
		return;
	}

	char				*memory = (char*)GlobalLock(handle);
	strcpy				(memory,string);
	GlobalUnlock		(handle);
	EmptyClipboard		();
	SetClipboardData	(CF_TEXT,handle);
	CloseClipboard		();
}

void update_clipboard	(const char *string)
{
#ifdef DEBUG
	if (IsDebuggerPresent())
		return;

	if (!OpenClipboard(0))
		return;

	HGLOBAL				handle = GetClipboardData(CF_TEXT);
	if (!handle) {
		CloseClipboard		();
		copy_to_clipboard	(string);
		return;
	}

	LPSTR				memory = (char*)GlobalLock(handle);
	u32					memory_length = xr_strlen(memory);
	u32					string_length = xr_strlen(string);
	LPSTR				buffer = (LPSTR)_alloca((memory_length + string_length + 1)*sizeof(char));
	strcpy				(buffer,memory);
	GlobalUnlock		(handle);

	strcat				(buffer,string);
	CloseClipboard		();
	copy_to_clipboard	(buffer);
#endif // DEBUG
}

extern void BuildStackTrace();
extern char g_stackTrace[100][4096];
extern int	g_stackTraceCount;

void LogStackTrace	(LPCSTR header)
{
	if (!shared_str_initialized)
		return;

	BuildStackTrace	();		

	Msg				("%s",header);

	for (int i=1; i<g_stackTraceCount; ++i)
		Msg			("%s",g_stackTrace[i]);
}

void gather_info		(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, LPSTR assertion_info)
{
	LPSTR				buffer = assertion_info;
	LPCSTR				endline = "\n";
	LPCSTR				prefix = "[error]";
	bool				extended_description = (description && !argument0 && strchr(description,'\n'));
	for (int i=0; i<2; ++i) {
		if (!i)
			buffer		+= sprintf(buffer,"%sFATAL ERROR%s%s",endline,endline,endline);
		buffer			+= sprintf(buffer,"%sExpression    : %s%s",prefix,expression,endline);
		buffer			+= sprintf(buffer,"%sFunction      : %s%s",prefix,function,endline);
		buffer			+= sprintf(buffer,"%sFile          : %s%s",prefix,file,endline);
		buffer			+= sprintf(buffer,"%sLine          : %d%s",prefix,line,endline);
		
		if (extended_description) {
			buffer		+= sprintf(buffer,"%s%s%s",endline,description,endline);
			if (argument0) {
				if (argument1) {
					buffer	+= sprintf(buffer,"%s%s",argument0,endline);
					buffer	+= sprintf(buffer,"%s%s",argument1,endline);
				}
				else
					buffer	+= sprintf(buffer,"%s%s",argument0,endline);
			}
		}
		else {
			buffer		+= sprintf(buffer,"%sDescription   : %s%s",prefix,description,endline);
			if (argument0) {
				if (argument1) {
					buffer	+= sprintf(buffer,"%sArgument 0    : %s%s",prefix,argument0,endline);
					buffer	+= sprintf(buffer,"%sArgument 1    : %s%s",prefix,argument1,endline);
				}
				else
					buffer	+= sprintf(buffer,"%sArguments     : %s%s",prefix,argument0,endline);
			}
		}

		buffer			+= sprintf(buffer,"%s",endline);
		if (!i) {
			if (shared_str_initialized) {
				Msg		("%s",assertion_info);
				FlushLog();
			}
			buffer		= assertion_info;
			endline		= "\r\n";
			prefix		= "";
		}
	}

	if (!IsDebuggerPresent() && !strstr(GetCommandLine(),"-no_call_stack_assert")) {
		if (shared_str_initialized)
			Msg			("stack trace:\n");

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
		buffer			+= sprintf(buffer,"stack trace:%s%s",endline,endline);
#endif // USE_OWN_ERROR_MESSAGE_WINDOW

		BuildStackTrace	();		

		for (int i=2; i<g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg		("%s",g_stackTrace[i]);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
			buffer		+= sprintf(buffer,"%s%s",g_stackTrace[i],endline);
#endif // USE_OWN_ERROR_MESSAGE_WINDOW
		}

		if (shared_str_initialized)
			FlushLog	();

		copy_to_clipboard	(assertion_info);
	}
}

void xrDebug::do_exit	(const std::string &message)
{
	FlushLog			();
	MessageBox			(NULL,message.c_str(),"Error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
	TerminateProcess	(GetCurrentProcess(),1);
}

void xrDebug::backend	(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, bool &ignore_always)
{
	static xrCriticalSection CS
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(xrDebug::backend))
#endif // PROFILE_CRITICAL_SECTIONS
	;

	CS.Enter			();

	error_after_dialog	= true;

	string4096			assertion_info;
	gather_info			(expression, description, argument0, argument1, file, line, function, assertion_info);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
	LPCSTR				endline = "\r\n";
	LPSTR				buffer = assertion_info + xr_strlen(assertion_info);
	buffer				+= sprintf(buffer,"%sPress CANCEL to abort execution%s",endline,endline);
	buffer				+= sprintf(buffer,"Press TRY AGAIN to continue execution%s",endline);
	buffer				+= sprintf(buffer,"Press CONTINUE to continue execution and ignore all the errors of this type%s%s",endline,endline);
#endif // USE_OWN_ERROR_MESSAGE_WINDOW

	if (handler)
		handler			();

	if (get_on_dialog())
		get_on_dialog()	(true);

#ifdef XRCORE_STATIC
	MessageBox			(NULL,assertion_info,"X-Ray error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
#else

	HWND hWnd = GetActiveWindow();
	if (hWnd == nullptr)
		hWnd = GetForegroundWindow();
	ShowWindow(hWnd, SW_MINIMIZE);

	int result = MessageBox(
		NULL, assertion_info, "Fatal Error",
		MB_CANCELTRYCONTINUE | MB_ICONERROR | MB_DEFBUTTON3 | MB_SYSTEMMODAL | MB_DEFAULT_DESKTOP_ONLY);

		switch (result) {
			case IDCANCEL : {
				DEBUG_INVOKE;
				// TODO: Maybe not correct
				exit(-1);
				break;
			}
			case IDTRYAGAIN : {
				error_after_dialog	= false;
				break;
			}
			case IDCONTINUE : {
				error_after_dialog	= false;
				ignore_always	= true;
				ShowWindow(hWnd, SW_SHOWNORMAL);
				break;
			}
			default: {
				Msg("! xrDebug::backend default reached");
				break;
			}
		}

#endif

	if (get_on_dialog())
		get_on_dialog()	(false);

	CS.Leave			();
}

LPCSTR xrDebug::error2string	(long code)
{
	LPCSTR				result	= 0;
	static	string1024	desc_storage;

#ifdef _M_AMD64
#else
	result				= DXGetErrorDescription(code);
#endif
	if (nullptr==result) 
	{
		FormatMessage	(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,desc_storage,sizeof(desc_storage)-1,0);
		result			= desc_storage;
	}
	return		result	;
}

void xrDebug::error		(long hr, const char* expr, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(error2string(hr),expr,0,0,file,line,function,ignore_always);
}

void xrDebug::error		(long hr, const char* expr, const char* e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(error2string(hr),expr,e2,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		("assertion failed",e1,0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const std::string &e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2.c_str(),0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,e3,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,e3,e4,file,line,function,ignore_always);
}

void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F,...)
{
	string1024	buffer;

	va_list		p;
	va_start	(p,F);
	vsprintf	(buffer,F,p);
	va_end		(p);

	bool		ignore_always = true;

	backend		("fatal error","<no expression>",buffer,0,file,line,function,ignore_always);
}

int out_of_memory_handler	(size_t size)
{
	Memory.mem_compact		();
	u32						process_heap	= mem_usage_impl(nullptr,nullptr);
	int						eco_strings		= (int)g_pStringContainer->stat_economy			();
	int						eco_smem		= (int)g_pSharedMemoryContainer->stat_economy	();
	Msg						("* [x-ray]: process heap[%d K]", process_heap / 1024);
	Msg						("* [x-ray]: economy: strings[%d K], smem[%d K]",eco_strings/1024,eco_smem);
	Debug.fatal				(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
	return					1;
}

extern LPCSTR log_name();

#if 1
extern void BuildStackTrace(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG ( __stdcall *PFNCHFILTFN ) ( EXCEPTION_POINTERS * pExPtrs ) ;
extern "C" BOOL __stdcall SetCrashHandlerFilter ( PFNCHFILTFN pFn );

static UnhandledExceptionFilterType	*previous_filter = 0;

#ifdef USE_OWN_MINI_DUMP
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );

void FlushLog				(LPCSTR file_name);

void save_mini_dump			(_EXCEPTION_POINTERS *pExceptionInfo)
{
	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll	= NULL;
	string_path		szDbgHelpPath;

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = strchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			strcpy	(pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			string_path	szDumpPath;
			string_path	szScratch;
			string64	t_stemp;

			timestamp	(t_stemp);
			strcpy		( szDumpPath, Core.ApplicationName);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, Core.UserName			);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, t_stemp				);
			strcat		( szDumpPath, ".mdmp"				);

			__try {
				if (FS.path_exist("$logs$"))
					FS.update_path	(szDumpPath,"$logs$",szDumpPath);
			}
            __except( EXCEPTION_EXECUTE_HANDLER ) {
				string_path	temp;
				strcpy		(temp,szDumpPath);
				strcpy		(szDumpPath,"logs/");
				strcat		(szDumpPath,temp);
            }

			string_path	log_file_name;
			strconcat	(sizeof(log_file_name), log_file_name, szDumpPath, ".log");
			FlushLog	(log_file_name);

			// create the file
			HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			if (INVALID_HANDLE_VALUE==hFile)	
			{
				// try to place into current directory
				MoveMemory	(szDumpPath,szDumpPath+5,strlen(szDumpPath));
				hFile		= ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			}
			if (hFile!=INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId				= ::GetCurrentThreadId();
				ExInfo.ExceptionPointers	= pExceptionInfo;
				ExInfo.ClientPointers		= NULL;

				// write the dump
				MINIDUMP_TYPE	dump_flags	= MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory );

				BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, NULL, NULL );
				if (bOK)
				{
					sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
					szResult = szScratch;
//					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
				::CloseHandle(hFile);
			}
			else
			{
				sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
				szResult = szScratch;
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}
}
#endif // USE_OWN_MINI_DUMP

void format_message	(LPSTR buffer, const u32 &buffer_size)
{
    LPVOID		message;
    DWORD		error_code = GetLastError(); 

	if (!error_code) {
		*buffer	= 0;
		return;
	}

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
		NULL
	);

	sprintf		(buffer,"[error][%8d]    : %s",error_code, (char*) message);
    LocalFree	(message);
}

LONG WINAPI UnhandledFilter	(_EXCEPTION_POINTERS *pExceptionInfo)
{
	string256				error_message;
	format_message			(error_message,sizeof(error_message));

	if (!error_after_dialog && !strstr(GetCommandLine(),"-no_call_stack_assert")) {
		CONTEXT				save = *pExceptionInfo->ContextRecord;
		BuildStackTrace		(pExceptionInfo);
		*pExceptionInfo->ContextRecord = save;

		if (shared_str_initialized)
			Msg				("stack trace:\n");
		copy_to_clipboard	("stack trace:\r\n\r\n");

		string4096			buffer;
		for (int i=0; i<g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg			("%s",g_stackTrace[i]);
			sprintf			(buffer,"%s\r\n",g_stackTrace[i]);
			update_clipboard(buffer);
		}

		if (*error_message) {
			if (shared_str_initialized)
				Msg			("\n%s",error_message);

			strcat			(error_message,"\r\n");
			update_clipboard(error_message);
		}
	}

	if (shared_str_initialized)
		FlushLog			();

#ifdef USE_OWN_MINI_DUMP
		save_mini_dump		(pExceptionInfo);
#endif // USE_OWN_MINI_DUMP
	if (!error_after_dialog) {
		if (Debug.get_on_dialog())
			Debug.get_on_dialog()	(true);

		MessageBox			(NULL,"Fatal error occured\n\nPress OK to abort program execution","Fatal error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
	}

	if (!previous_filter) {
#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
		if (Debug.get_on_dialog())
			Debug.get_on_dialog()	(false);
#endif // USE_OWN_ERROR_MESSAGE_WINDOW

		return				(EXCEPTION_CONTINUE_SEARCH) ;
	}

	previous_filter			(pExceptionInfo);

#ifdef USE_OWN_ERROR_MESSAGE_WINDOW
	if (Debug.get_on_dialog())
		Debug.get_on_dialog()		(false);
#endif // USE_OWN_ERROR_MESSAGE_WINDOW

	return					(EXCEPTION_CONTINUE_SEARCH) ;
}
#endif

//////////////////////////////////////////////////////////////////////
#ifdef M_BORLAND
	namespace std{
		extern new_handler _RTLENTRY _EXPFUNC set_new_handler( new_handler new_p );
	};

	static void __cdecl def_new_handler() 
    {
		FATAL		("Out of memory.");
    }

    void	xrDebug::_initialize		(const bool &dedicated)
    {
		handler							= 0;
		m_on_dialog						= 0;
        std::set_new_handler			(def_new_handler);	// exception-handler for 'out of memory' condition
//		::SetUnhandledExceptionFilter	(UnhandledFilter);	// exception handler to all "unhandled" exceptions
    }
#else
    typedef int		(__cdecl * _PNH)( size_t );

	void _terminate		()
	{
		if (strstr(GetCommandLine(),"-silent_error_mode"))
			exit				(-1);

		string4096				assertion_info;
		
		gather_info				(
			"<no expression>",
			"Unexpected application termination",
			0,
			0,
	#ifdef _ANONYMOUS_BUILD
			"",
			0,
	#else
			__FILE__,
			__LINE__,
	#endif
	#ifndef _EDITOR
			__FUNCTION__,
	#else // _EDITOR
			"",
	#endif // _EDITOR
			assertion_info
		);
		
		LPCSTR					endline = "\r\n";
		LPSTR					buffer = assertion_info + xr_strlen(assertion_info);
		buffer					+= sprintf(buffer, (const char*) strlen(assertion_info), "Press OK to abort execution%s", endline);

		MessageBox				(
			/*GetTopWindow(NULL)*/ nullptr,
			assertion_info,
			"Fatal Error",
			MB_OK|MB_ICONERROR|MB_SYSTEMMODAL
		);
		
		exit					(-1);
	}

	void debug_on_thread_spawn			()
	{
		std::set_terminate				(_terminate);
	}

	static void handler_base				(LPCSTR reason_string)
	{
		bool							ignore_always = false;
		Debug.backend					(
			"error handler is invoked!",
			reason_string,
			0,
			0,
			DEBUG_INFO,
			ignore_always
		);
	}

	static void invalid_parameter_handler	(
			const wchar_t *expression,
			const wchar_t *function,
			const wchar_t *file,
			unsigned int line,
			uintptr_t reserved
		)
	{
		bool							ignore_always = false;

		string4096						expression_;
		string4096						function_;
		string4096						file_;
		size_t							converted_chars = 0;
//		errno_t							err = 
		if (expression)
			wcstombs_s	(
				&converted_chars, 
				expression_,
				sizeof(expression_),
				expression,
				(wcslen(expression) + 1)*2*sizeof(char)
			);
		else
			strcpy_s					(expression_,"");

		if (function)
			wcstombs_s	(
				&converted_chars, 
				function_,
				sizeof(function_),
				function,
				(wcslen(function) + 1)*2*sizeof(char)
			);
		else
			strcpy_s					(function_,__FUNCTION__);

		if (file)
			wcstombs_s	(
				&converted_chars, 
				file_,
				sizeof(file_),
				file,
				(wcslen(file) + 1)*2*sizeof(char)
			);
		else {
			line						= __LINE__;
			strcpy_s					(file_,__FILE__);
		}

		Debug.backend					(
			"error handler is invoked!",
			expression_,
			0,
			0,
			file_,
			line,
			function_,
			ignore_always
		);
	}

	static void std_out_of_memory_handler	()
	{
		handler_base					("std: out of memory");
	}

	static void pure_call_handler			()
	{
		handler_base					("pure virtual function call");
	}

#ifdef CS_USE_EXCEPTIONS
	static void unexpected_handler			()
	{
		handler_base					("unexpected program termination");
	}
#endif // CS_USE_EXCEPTIONS

	static void abort_handler				(int signal)
	{
		handler_base					("application is aborting");
	}

	static void floating_point_handler		(int signal)
	{
		handler_base					("floating point error");
	}

	static void illegal_instruction_handler	(int signal)
	{
		handler_base					("illegal instruction");
	}

//	static void storage_access_handler		(int signal)
//	{
//		handler_base					("illegal storage access");
//	}

	static void termination_handler			(int signal)
	{
		handler_base					("termination with exit code 3");
	}

    void	xrDebug::_initialize		(const bool &dedicated)
    {
		debug_on_thread_spawn			();

		_set_abort_behavior				(0,_WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		signal							(SIGABRT,		abort_handler);
		signal							(SIGABRT_COMPAT,abort_handler);
		signal							(SIGFPE,		floating_point_handler);
		signal							(SIGILL,		illegal_instruction_handler);
		signal							(SIGINT,		0);
//		signal							(SIGSEGV,		storage_access_handler);
		signal							(SIGTERM,		termination_handler);

		_set_invalid_parameter_handler	(&invalid_parameter_handler);

		_set_new_mode					(1);
		_set_new_handler				(&out_of_memory_handler);
		std::set_new_handler			(&std_out_of_memory_handler);

		_set_purecall_handler			(&pure_call_handler);

#if 0// should be if we use exceptions
		std::set_unexpected				(_terminate);
#endif

		previous_filter					= ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions

#if 0
		struct foo {static void	recurs	(const u32 &count)
		{
			if (!count)
				return;

			_alloca			(4096);
			recurs			(count - 1);
		}};
		foo::recurs			(u32(-1));
		std::terminate		();
#endif // 0
	}
#endif