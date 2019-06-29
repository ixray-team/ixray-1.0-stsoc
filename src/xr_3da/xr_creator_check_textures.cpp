#include "stdafx.h"
#include "xr_creator.h"

void CCreator::LL_CheckTextures()
{
	u32	m_base,c_base,m_lmaps,c_lmaps;
	Device.Shader._GetMemoryUsage		(m_base,c_base,m_lmaps,c_lmaps);

	Msg	("* t-report - base: %d, %d K",	c_base,		m_base/1024);
	Msg	("* t-report - lmap: %d, %d K",	c_lmaps,	m_lmaps/1024);
	BOOL	bError	= FALSE;
	if (m_base>64*1024*1024 || c_base>400)
	{
		LPCSTR msg	= "Too many base-textures (limit: 400 textures or 64M).\n        Reduce number of textures (better) or their resolution (worse).";
		Msg			("***FATAL***: %s",msg);
		bError		= TRUE;
	}
	if (m_lmaps>32*1024*1024 || c_lmaps>8)
	{
		LPCSTR msg	= "Too many lmap-textures (limit: 8 textures or 32M).\n        Reduce pixel density (worse) or use more vertex lighting (better).";
		Msg			("***FATAL***: %s",msg);
		bError		= TRUE;
	}
	if (bError && 0==strstr(Core.Params,"-external"))
	{
		BOOL	bAccess	= FALSE;
		BOOL	bKoan	= FALSE;
		BOOL	bProf	= FALSE;
		BOOL	bOles	= FALSE;
		BOOL	bAlexMX	= FALSE;
		if ((0==stricmp(Core.UserName,"Andrew"))	&&(0==stricmp(Core.CompName,"Prof")))		bProf = TRUE;
		if ((0==stricmp(Core.UserName,"Koan"))		&&(0==stricmp(Core.CompName,"Koan")))		bKoan = TRUE;
		if ((0==stricmp(Core.UserName,"Oles"))		&&(0==stricmp(Core.CompName,"OlesXXX")))	bOles = TRUE;
		if ((0==stricmp(Core.UserName,"Alexmx"))	&&(0==stricmp(Core.CompName,"Alexmx")))		bAlexMX = TRUE;
		bAccess			= bKoan || bProf || bOles || bAlexMX;
		if (!bAccess)	Debug.fatal	("Too much memory used for textures.");
	}
}
