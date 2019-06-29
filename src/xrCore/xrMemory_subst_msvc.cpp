#include "stdafx.h"
#pragma hdrstop

#include	"xrMemory_align.h"

#ifndef		__BORLANDC__

#ifndef DEBUG_MEMORY_MANAGER
#	define	debug_mode 0
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
	XRCORE_API void*	g_globalCheckAddr = NULL;
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
	extern void save_stack_trace	();
#endif // DEBUG_MEMORY_MANAGER

MEMPOOL		mem_pools			[mem_pools_count];

// MSVC
ICF	u8*		acc_header			(void* P)	{	u8*		_P		= (u8*)P;	return	_P-1;	}
ICF	u32		get_header			(void* P)	{	return	(u32)*acc_header(P);				}
ICF	u32		get_pool			(size_t size)
{
	u32		pid					= u32(size/mem_pools_ebase);
	if (pid>=mem_pools_count)	return mem_generic;
	else						return pid;
}

void*	xrMemory::mem_alloc		(size_t size
#ifdef DEBUG_MEMORY_MANAGER
								 , const char* _name
#endif // DEBUG_MEMORY_MANAGER
								 )
{
	// if (mem_initialized)		Memory.dbg_check			();
	stat_calls++;

#ifdef DEBUG_MEMORY_MANAGER
	//if (_name && (0==strcmp(_name,"C++ NEW")) && (12==size))
	//{
	//	debug_cs.Enter		();
	//	debug_cs.Leave		();
	//}
#endif // DEBUG_MEMORY_MANAGER
	// if (size>14310800 && size<14310860)	__asm int 3;
#ifdef DEBUG_MEMORY_MANAGER
	if (mem_initialized)		debug_cs.Enter		();
#endif // DEBUG_MEMORY_MANAGER
	u32		_footer				=	debug_mode?4:0;
	void*	_ptr				=	0;

	//
	if (!mem_initialized /*|| debug_mode*/)		
	{
		// generic
		void*	_real			=	xr_aligned_offset_malloc	(size + _footer, 16, 0x1);
		_ptr					=	(void*)(((u8*)_real)+1);
		*acc_header(_ptr)		=	mem_generic;
	} else {
#ifdef DEBUG_MEMORY_MANAGER
		save_stack_trace		();
#endif // DEBUG
		//	accelerated
		u32	pool				=	get_pool	(size+_footer);
		if (mem_generic==pool)	
		{
			// generic
			void*	_real		=	xr_aligned_offset_malloc	(size + _footer,16,0x1);
			_ptr				=	(void*)(((u8*)_real)+1);
			*acc_header(_ptr)	=	mem_generic;
		} else {
			// pooled
			void*	_real		=	mem_pools[pool].create();
			_ptr				=	(void*)(((u8*)_real)+1);
			*acc_header(_ptr)	=	(u8)pool;
		}
	}

#ifdef DEBUG_MEMORY_MANAGER
	if		(debug_mode)		dbg_register		(_ptr,size,_name);
	if (mem_initialized)		debug_cs.Leave		();
	//if(g_globalCheckAddr==_ptr){
	//	__asm int 3;
	//}
	//if (_name && (0==strcmp(_name,"class ISpatial *")) && (size==376))
	//{
	//	__asm int 3;
	//}
#endif // DEBUG_MEMORY_MANAGER
	return	_ptr;
}

void	xrMemory::mem_free		(void* P)
{
#ifdef DEBUG_MEMORY_MANAGER
	if(g_globalCheckAddr==P)
		__asm int 3;
#endif // DEBUG_MEMORY_MANAGER
	

	stat_calls++;
#ifdef DEBUG_MEMORY_MANAGER
	if (mem_initialized)		debug_cs.Enter		();
#endif // DEBUG_MEMORY_MANAGER
	if		(debug_mode)		dbg_unregister	(P);
	u32	pool					= get_header	(P);
	void* _real					= (void*)(((u8*)P)-1);
	if (mem_generic==pool)		
	{
		// generic
		xr_aligned_free			(_real);
	} else {
		// pooled
		VERIFY2					(pool<mem_pools_count,"Memory corruption");
		mem_pools[pool].destroy	(_real);
	}
#ifdef DEBUG_MEMORY_MANAGER
	if (mem_initialized)		debug_cs.Leave	();
#endif // DEBUG_MEMORY_MANAGER
}

extern BOOL	g_bDbgFillMemory	;
void*	xrMemory::mem_realloc	(void* P, size_t size
#ifdef DEBUG_MEMORY_MANAGER
								 , const char* _name
#endif // DEBUG_MEMORY_MANAGER
								 )
{
	stat_calls++;
	if (0==P)					return mem_alloc	(size
#ifdef DEBUG_MEMORY_MANAGER
		,_name
#endif // DEBUG_MEMORY_MANAGER
		);

#ifdef DEBUG_MEMORY_MANAGER
	if(g_globalCheckAddr==P)
		__asm int 3;
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
	if (mem_initialized)		debug_cs.Enter		();
#endif // DEBUG_MEMORY_MANAGER
	u32		p_current			= get_header(P);
	u32		p_new				= get_pool	(size+(debug_mode?4:0));
	u32		p_mode				;

	if (mem_generic==p_current)	{
		if (p_new<p_current)		p_mode	= 2	;
		else						p_mode	= 0	;
	} else 							p_mode	= 1	;

	void*	_real				= (void*)(((u8*)P)-1);
	void*	_ptr				= NULL;
	if		(0==p_mode)
	{
		u32		_footer			=	debug_mode?4:0;
#ifdef DEBUG_MEMORY_MANAGER
		if		(debug_mode)	{
			g_bDbgFillMemory	= false;
			dbg_unregister		(P);
			g_bDbgFillMemory	= true;
		}
#endif // DEBUG_MEMORY_MANAGER
		void*	_real2			=	xr_aligned_offset_realloc	(_real,size+_footer,16,0x1);
		_ptr					= (void*)(((u8*)_real2)+1);
		*acc_header(_ptr)		= mem_generic;
#ifdef DEBUG_MEMORY_MANAGER
		if		(debug_mode)	dbg_register	(_ptr,size,_name);
#endif // DEBUG_MEMORY_MANAGER
	} else if (1==p_mode)		{
		// pooled realloc
		R_ASSERT2				(p_current<mem_pools_count,"Memory corruption");
		u32		s_current		= mem_pools[p_current].get_element();
		u32		s_dest			= (u32)size;
		void*	p_old			= P;
		void*	p_new			= mem_alloc		(size
#ifdef DEBUG_MEMORY_MANAGER
			,_name
#endif // DEBUG_MEMORY_MANAGER
			);
		mem_copy				(p_new,p_old,_min(s_current,s_dest));
		mem_free				(p_old);
		_ptr					= p_new;
	} else if (2==p_mode)		{
		// relocate into another mmgr(pooled) from real
		void*	p_old			= P;
		void*	p_new			= mem_alloc		(size
#ifdef DEBUG_MEMORY_MANAGER
			,_name
#endif // DEBUG_MEMORY_MANAGER
			);
		mem_copy				(p_new,p_old,(u32)size);
		mem_free				(p_old);
		_ptr					= p_new;
	}

#ifdef DEBUG_MEMORY_MANAGER
	if (mem_initialized)		debug_cs.Leave	();

	if(g_globalCheckAddr==_ptr)
		__asm int 3;
#endif // DEBUG_MEMORY_MANAGER

	return	_ptr;
}

#endif
