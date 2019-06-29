//#include "stdafx.h"
//#pragma hdrstop
#include <malloc.h>
#include "xr_print.h"
#include "xr_LuaAllocator.h"

size_t	__cdecl		lua_memusage	()
{
//	mallinfo	_i	= dlmallinfo	();
	return		0;	//_i.uordblks;
}

//extern "C"	{
void	stat_hook	(size_t bytes)
{
//		Log			("",u32(bytes));
}
//};
