#include "stdafx.h"
#pragma hdrstop

#include "xrdebug.h"

#include "dxerr9.h"

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
#endif

#ifndef _M_AMD64
#	ifndef __BORLANDC__
#		pragma comment(lib,"dxerr9.lib")
#	endif
#endif

#include <dbghelp.h>					// MiniDump flags
#include "BugTrap.h"					// for BugTrap functionality
#pragma comment(lib,"BugTrap.lib")		// Link to ANSI DLL

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
}

extern void BuildStackTrace ();
extern char g_stackTrace[100][4096];
extern int g_stackTraceCount;

void XRCORE_API LogStackTrace	(LPCSTR header)
{
	if (!shared_str_initialized)
		return;

	BuildStackTrace	();		

	Msg				("%s",header);

	for (int i=1; i<g_stackTraceCount; ++i)
		Msg			("%s",g_stackTrace[i]);
}

void xrDebug::backend(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, bool &ignore_always)
{
	static xrCriticalSection CS
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(xrDebug::backend))
#endif // PROFILE_CRITICAL_SECTIONS
	;

	CS.Enter			();

	error_after_dialog	= true;

	string4096			assertion_info;
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

	if (!IsDebuggerPresent() && !strstr(Core.Params,"-no_call_stack_assert")) {
		if (shared_str_initialized)
			Msg			("stack trace:\n");
		buffer			+= sprintf(buffer,"stack trace:%s%s",endline,endline);

		BuildStackTrace	();		

		for (int i=2; i<g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg		("%s",g_stackTrace[i]);

			buffer		+= sprintf(buffer,"%s%s",g_stackTrace[i],endline);
		}

		if (shared_str_initialized)
			FlushLog	();

		copy_to_clipboard	(assertion_info);
	}

	buffer				+= sprintf(buffer,"%sPress ABORT to abort execution%s",endline,endline);
	buffer				+= sprintf(buffer,"Press RETRY to continue execution%s",endline);
	buffer				+= sprintf(buffer,"Press IGNORE to continue execution and ignore all the errors of this type%s%s",endline,endline);

	if (handler)
		handler			();

	if (get_on_dialog())
		get_on_dialog()	(true);

#ifdef XRCORE_STATIC
	MessageBox			(NULL,tmp,"X-Ray error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
#else
	int					result = 
		MessageBox(
			GetTopWindow(NULL),
			assertion_info,
			"Fatal Error",
			MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_SYSTEMMODAL
		);

	switch (result) {
		case IDABORT : {
			DEBUG_INVOKE;
			break;
		}
		case IDRETRY : {
			error_after_dialog	= false;
			break;
		}
		case IDIGNORE : {
			error_after_dialog	= false;
			ignore_always	= true;
			break;
		}
		default : NODEFAULT;
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
	result				= DXGetErrorDescription9	(code);
#endif
	if (0==result) 
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

int __cdecl _out_of_memory	(size_t size)
{
	Debug.fatal				(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
	return					1;
}

void __cdecl _terminate		()
{
	FATAL					("Unexpected application termination");
}

extern LPCSTR log_name();

void CALLBACK PreErrorHandler	(INT_PTR)
{
	string256				log_folder;
	FS.update_path			(log_folder,"$logs$","");
	if ((log_folder[0] != '\\') && (log_folder[1] != ':')) {
		string256			current_folder;
		_getcwd				(current_folder,sizeof(current_folder));
		
		string256			relative_path;
		strcpy				(relative_path,log_folder);
		strconcat			(log_folder,current_folder,"\\",relative_path);
	}

	BT_SetReportFilePath	(log_folder);
	BT_AddLogFile			(log_name());
}

void SetupExceptionHandler	()
{
	BT_SetPreErrHandler		(PreErrorHandler,0);
	BT_SetAppName			("XRay Engine");
	BT_SetActivityType		(BTA_SAVEREPORT);
	BT_SetReportFormat		(BTRF_TEXT);
	BT_SetFlags				(BTF_DETAILEDMODE | /**BTF_EDIETMAIL | /**/BTF_ATTACHREPORT /**| BTF_LISTPROCESSES /**/| BTF_SHOWADVANCEDUI /**| BTF_SCREENCAPTURE/**/);
	BT_SetDumpType			(
		MiniDumpNormal |
//		MiniDumpWithDataSegs |
//		MiniDumpWithFullMemory |
//		MiniDumpWithHandleData |
//		MiniDumpFilterMemory |
//		MiniDumpScanMemory |
//		MiniDumpWithUnloadedModules |
//.		MiniDumpWithIndirectlyReferenced |
//		MiniDumpFilterModulePaths |
//		MiniDumpWithProcessThreadData |
//		MiniDumpWithPrivateReadWriteMemory |
//		MiniDumpWithoutOptionalData |
//.		MiniDumpWithFullMemoryInfo |
//.		MiniDumpWithThreadInfo |
//.		MiniDumpWithCodeSegs |
		0
	);
//	BT_SetSupportEMail		("iassenev@gmail.com");
//	BT_SetSupportServer		("localhost", 9999);
//	BT_SetSupportURL		("http://www.your-web-site.com");
}

#if 1
extern void BuildStackTrace(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG ( __stdcall *PFNCHFILTFN ) ( EXCEPTION_POINTERS * pExPtrs ) ;
extern "C" BOOL __stdcall SetCrashHandlerFilter ( PFNCHFILTFN pFn );

static UnhandledExceptionFilterType	*previous_filter = 0;

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );


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

			// work out a good place for the dump file
			timestamp	(t_stemp);
			strcpy		( szDumpPath, "logs\\"				);
			strcat		( szDumpPath, Core.ApplicationName	);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, Core.UserName			);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, t_stemp				);
			strcat		( szDumpPath, ".mdmp"				);

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

LONG WINAPI UnhandledFilter	(_EXCEPTION_POINTERS *pExceptionInfo)
{
	if (!error_after_dialog && !strstr(Core.Params,"-no_call_stack_assert")) {
		CONTEXT				save = *pExceptionInfo->ContextRecord;
		BuildStackTrace		(pExceptionInfo);
		*pExceptionInfo->ContextRecord = save;

		if (shared_str_initialized)
			Msg				("stack trace:\n");
		copy_to_clipboard	("stack trace:\r\n\r\n");

		string4096			buffer;
		for (int i=2; i<g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg			("%s",g_stackTrace[i]);
			sprintf			(buffer,"%s\r\n",g_stackTrace[i]);
			update_clipboard(buffer);
		}
	}

	if (shared_str_initialized)
		FlushLog			();

#ifndef DEBUG
	save_mini_dump			(pExceptionInfo);
#endif

	if (!error_after_dialog) {
		if (Debug.get_on_dialog())
			Debug.get_on_dialog()	(true);

		MessageBox			(NULL,"Fatal error occured\n\nPress OK to abort program execution","Fatal error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
	}

	if (!previous_filter) {
		if (Debug.get_on_dialog())
			Debug.get_on_dialog()	(false);

		return				(EXCEPTION_CONTINUE_SEARCH) ;
	}

	previous_filter			(pExceptionInfo);

	if (Debug.get_on_dialog())
		Debug.get_on_dialog()		(false);

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

    void	xrDebug::_initialize		()
    {
		handler							= 0;
		m_on_dialog						= 0;
        std::set_new_handler			(def_new_handler);	// exception-handler for 'out of memory' condition
//		::SetUnhandledExceptionFilter	(UnhandledFilter);	// exception handler to all "unhandled" exceptions
    }
#else
    typedef int		(__cdecl * _PNH)( size_t );
    _CRTIMP int		__cdecl _set_new_mode( int );
    _CRTIMP _PNH	__cdecl _set_new_handler( _PNH );

    void	xrDebug::_initialize		()
    {
		handler							= 0;
        _set_new_mode					(1);					// gen exception if can't allocate memory
        _set_new_handler				(_out_of_memory	);		// exception-handler for 'out of memory' condition
		std::set_terminate				(_terminate);
		std::set_unexpected				(_terminate);
		SetupExceptionHandler			();
//		SetCrashHandlerFilter			(UnhandledFilter);
		previous_filter					= ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions
	}
#endif