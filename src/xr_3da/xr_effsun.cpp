#include "stdafx.h"
#include "igame_level.h"

#include "xr_effsun.h"
#include "environment.h"

CSun::CSun( CInifile* pIni, LPCSTR section ) {
	// общие установки
	sun_color.set				(255, 255, 255, 255);
	sun_dir.set					(-0.5774f,-0.5774f,-0.5774f);
	
	LoadSunData					( pIni, section );
	LensFlare.Load				( pIni, section );
}

CSun::~CSun	( ) {
}

void CSun::LoadSunData	( CInifile* pIni, LPCSTR section ) {
	VERIFY( section );
	sun_color			= pIni->r_fcolor	( section, "sun_color" );
	sun_dir				= pIni->r_fvector3	( section, "sun_dir"   );
	sun_dir.normalize	();
}

void CSun::RenderSource( ) {
	LensFlare.Render	(true,false,false);
}

void CSun::RenderFlares( ) {
	LensFlare.Render	(false,true,true);
}

void CSun::Update() 
{
	LensFlare.Update(sun_dir,sun_color);
}
