#ifndef __XR_EFFSUN_H__
#define __XR_EFFSUN_H__

#include "xr_efflensflare.h"

class							CEffectLensFlare;
//-----------------------------------------------------------------------------------------------------------
//Sun Effect
//-----------------------------------------------------------------------------------------------------------
class							CSun
{
	Fcolor						sun_color;
	Fvector						sun_dir;
	CLensFlare					LensFlare;
	void						LoadSunData				( CInifile* pIni, LPCSTR section );
public:
								CSun					( CInifile* pIni, LPCSTR section );
								~CSun					( );

	void __fastcall				RenderSource			( );
	void __fastcall				RenderFlares			( );
	Fvector&					Direction				( ) { return sun_dir;	}
	Fcolor&						Color					( )	{ return sun_color;	}
	void						Update					( );
};

#endif //__XR_EFFSUN_H__
