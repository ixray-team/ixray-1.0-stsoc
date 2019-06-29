#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_bloom_filter::CBlender_bloom_filter	()	{	description.CLS		= 0;	}
CBlender_bloom_filter::~CBlender_bloom_filter	()	{	}
 
void	CBlender_bloom_filter::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch ()
	C.r2_Pass				("null",			"r2_combine",	FALSE,	FALSE);
	C.r2_Sampler_rtf		("s_bloom",			r2_RT_N_H);
	C.r2_Sampler_rtf		("s_color",			r2_RT_D_G);
	C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
	C.r2_End				();
}
