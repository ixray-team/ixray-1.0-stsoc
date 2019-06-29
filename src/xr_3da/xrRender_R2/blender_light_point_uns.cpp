#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point_uns.h"

CBlender_accum_point_uns::CBlender_accum_point_uns	()	{	description.CLS		= 0;	}
CBlender_accum_point_uns::~CBlender_accum_point_uns	()	{	}

void	CBlender_accum_point_uns::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// masking
		C.r_Pass			("accum_mask",			"null",				false,TRUE,FALSE);
		C.r_Sampler_rtf		("s_position",			r2_RT_P);
		C.r_Sampler_rtf		("s_normal",			r2_RT_N_H);
		C.r_End				();
		break;
	case 1:	// lighting/shadowing - front/back
	case 2:
	case 3:
		C.r_Pass			("accum_volume",		"accum_point_uns",	false,FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",			r2_RT_P);
		C.r_Sampler_rtf		("s_normal",			r2_RT_N_H);
		C.r_Sampler_clf		("s_material",			r2_material);
		C.r_Sampler_clf		("s_attenuate",			r2_attenuate);
		C.r_End				();
		break;
	}
}
