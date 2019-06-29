#include "stdafx.h"
#pragma hdrstop

#include "Blender_decompress.h"

CBlender_decompress::CBlender_decompress	()	{	description.CLS		= 0;	}
CBlender_decompress::~CBlender_decompress	()	{	}	
 
void	CBlender_decompress::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// normal (MRT decompress)
		C.r_Pass			("null",			"decompress",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_depth,	false	);
		C.r_Sampler_rtf		("s_normal",		r2_RT_accum,	false	);
		C.r_End				();
		break;
	case 1:	// pass 0
		C.r_Pass			("null",			"decompress_0",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_depth,	false	);
		C.r_Sampler_rtf		("s_normal",		r2_RT_accum,	false	);
		C.r_End				();
		break;
	case 2:	// pass 1
		C.r_Pass			("null",			"decompress_1",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_depth,	false	);
		C.r_Sampler_rtf		("s_normal",		r2_RT_accum,	false	);
		C.r_End				();
		break;
	}
}
