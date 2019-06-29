#include "stdafx_.h"
#include "BugSlayerUtil.h"
#include <stdio.h>

#define MAX_STACK_TRACE	100

char g_stackTrace[MAX_STACK_TRACE][4096];
int g_stackTraceCount = 0;

void BuildStackTrace	(struct _EXCEPTION_POINTERS *g_BlackBoxUIExPtrs)
{
	FillMemory			(g_stackTrace[0],MAX_STACK_TRACE*256, 0 );

	const TCHAR* traceDump = 
		GetFirstStackTraceString( GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE,
									g_BlackBoxUIExPtrs );
	g_stackTraceCount = 0;

	int incr = 85;
	while ( NULL != traceDump ) {
		int				length = strlen(traceDump);
		if (length < 4096)
			lstrcpy		(g_stackTrace[g_stackTraceCount], traceDump);
		else {
			memcpy		(g_stackTrace[g_stackTraceCount],traceDump,4092);
			char		*i = g_stackTrace[g_stackTraceCount] + 4092;
			*i++		= '.';
			*i++		= '.';
			*i++		= '.';
			*i			= 0;
		}
	
		g_stackTraceCount++;

		incr += 2;
		traceDump = GetNextStackTraceString( GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE,
			g_BlackBoxUIExPtrs );		
	}
}

#if 0
#	pragma auto_inline(off)
	DWORD_PTR program_counter()
	{
		DWORD_PTR programcounter;

		// Get the return address out of the current stack frame
		__asm mov eax, [ebp + 4]
		// Put the return address into the variable we'll return
		__asm mov [programcounter], eax

		return programcounter;
	}
#	pragma auto_inline(on)
#else
	extern "C" void * _ReturnAddress(void);

#	pragma intrinsic(_ReturnAddress)

#	pragma auto_inline(off)
	DWORD_PTR program_counter()
	{
		return (DWORD_PTR)_ReturnAddress();
	}
#	pragma auto_inline(on)
#endif

void BuildStackTrace	()
{
    CONTEXT					context;
	context.ContextFlags	= CONTEXT_FULL;
	if (!GetThreadContext(GetCurrentThread(),&context))
		return;

	context.Eip				= program_counter();
	__asm					mov context.Ebp, ebp
	__asm					mov context.Esp, esp

	EXCEPTION_POINTERS		ex_ptrs;
	ex_ptrs.ContextRecord	= &context;
	ex_ptrs.ExceptionRecord	= 0;

	BuildStackTrace			(&ex_ptrs);
}

__declspec(noinline)
void OutputDebugStackTrace	(const char *header)
{
	BuildStackTrace			();		

	if (header) {
		OutputDebugString	(header);
		OutputDebugString	(":\r\n");
	}

	for (int i=2; i<g_stackTraceCount; ++i) {
		OutputDebugString	(g_stackTrace[i]);
		OutputDebugString	("\r\n");
	}
}
