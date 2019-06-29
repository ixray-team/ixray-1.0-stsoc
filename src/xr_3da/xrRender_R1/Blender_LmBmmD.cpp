// BlenderDefault.cpp: implementation of the CBlender_LmBmmD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_LmBmmD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LmBmmD::CBlender_LmBmmD	()
{
	description.CLS		= B_LmBmmD;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_LmBmmD::~CBlender_LmBmmD	()
{
	
}

void	CBlender_LmBmmD::Save		(IWriter& fs )
{
	IBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Detail map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
}
void	CBlender_LmBmmD::Load		(IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmBmmD::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base [*] diffuse
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
			C.StageEnd				();
			
			// Stage2 - Second texture
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
			C.StageSET_TMC			(oT2_Name,oT2_xform,"$null",0);
			C.StageEnd				();
		}
		C.PassEnd			();
	} else {
		if (2==C.iElement)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET		();
				C.PassSET_LightFog		(FALSE,FALSE);
				
				// Stage0 - Lightmap
				C.StageBegin			();
				C.StageTemplate_LMAP0	();
				C.StageEnd				();
			}
			C.PassEnd			();
		} else {
			switch (HW.Caps.pixel.dwStages)
			{
			case 2:		// Geforce1/2/MX
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,TRUE);
					C.PassSET_Blend_SET		();
					C.PassSET_LightFog		(FALSE,TRUE);
					
					// Stage0 - [=] Lightmap
					C.StageBegin			();
					C.StageTemplate_LMAP0	();
					C.StageEnd				();
					
					// Stage1 - [**] Base texture
					C.StageBegin			();
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
					C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
					C.StageEnd				();
				}
				C.PassEnd			();
				
				// Pass2 - Detail
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,FALSE);
					C.PassSET_Blend_MUL2X	();
					C.PassSET_LightFog		(FALSE,TRUE);
					
					// Stage0 - [**] Detail
					C.StageBegin			();
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.StageSET_TMC			(oT2_Name,oT2_xform,"$null",0);
					C.StageEnd				();
				}
				C.PassEnd			();
				break;
			case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
			default:
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,TRUE);
					C.PassSET_Blend_SET		();
					C.PassSET_LightFog		(FALSE,TRUE);
					
					// Stage0 - Lightmap
					C.StageBegin			();
					C.StageTemplate_LMAP0	();
					C.StageEnd				();
					
					// Stage1 - Base texture
					C.StageBegin			();
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
					C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
					C.StageEnd				();
					
					// Stage2 - Detail
					C.StageBegin			();
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
					C.StageSET_TMC			(oT2_Name,oT2_xform,"$null",0);
					C.StageEnd				();
				}
				C.PassEnd			();
				break;
			}
		}
	}
}
