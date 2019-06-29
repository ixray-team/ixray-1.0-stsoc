// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HUDCrosshair.h"
#include "UIStaticItem.h"

CHUDCrosshair::CHUDCrosshair	()
{
	hGeomLine.create			(FVF::F_TL0uv,RCache.Vertex.Buffer(),0);
	hShader.create				("editor\\wire");

	//вычислить и запомнить центр экрана
//	center.set(int(Device.dwWidth)/2,int(Device.dwHeight)/2);
	radius = 0;
}


CHUDCrosshair::~CHUDCrosshair	()
{
	hGeomLine.destroy			();
	hShader.destroy				();
}

void CHUDCrosshair::Load		()
{
	//все размеры в процентах от длины экрана
	//длина крестика 
	cross_length_perc = pSettings->r_float (HUD_CURSOR_SECTION, "cross_length");
//	cross_length = iFloor(0.5f + cross_length_perc*float(Device.dwWidth));

	min_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "min_radius");
	//min_radius = iFloor(0.5f + min_radius_perc*float(Device.dwWidth));

	max_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "max_radius");
	//max_radius = iFloor(0.5f + max_radius_perc*float(Device.dwWidth));

	cross_color = pSettings->r_fcolor (HUD_CURSOR_SECTION, "cross_color").get();


	radius_speed_perc = pSettings->r_float (HUD_CURSOR_SECTION, "radius_lerp_speed");
}

//выставляет radius от min_radius до max_radius
void CHUDCrosshair::SetDispersion	(float disp)
{ 
	Fvector4 r;
	Fvector R			= { VIEWPORT_NEAR*_sin(disp), 0.f, VIEWPORT_NEAR };
	Device.mProject.transform	(r,R);

	float radius_pixels		= _abs(r.x)*Device.fWidth_2;
	//	clamp(radius_pixels, min_radius, max_radius);
	target_radius		= radius_pixels; 
/*
	Fvector E={0,0,0};
	Fvector D={0,0,1}, R={1,0,0};
	E.mad(D,_cos(disp));
	E.mad(R,_sin(disp));
	Device.mProject.transform_tiny(E);

    int radius_pixels = iFloor(0.5f + _abs(E.x)*Device.fWidth_2);
//	clamp(radius_pixels, min_radius, max_radius);

	target_radius = radius_pixels; 
*/
}

extern ENGINE_API BOOL g_bRendering; 
void CHUDCrosshair::OnRender ()
{
	VERIFY			(g_bRendering);
	Fvector2		center;
	center.set		(Device.dwWidth/2.0f, Device.dwHeight/2.0f);

	// draw back
	u32			dwOffset,dwCount;
	FVF::TL0uv* pv_start				= (FVF::TL0uv*)RCache.Vertex.Lock(10,hGeomLine->vb_stride,dwOffset);
	FVF::TL0uv* pv						= pv_start;
	

	float cross_length					= cross_length_perc*Device.dwWidth;
	float min_radius					= min_radius_perc*Device.dwWidth;
	float max_radius					= max_radius_perc*Device.dwWidth;

	clamp								(target_radius , min_radius, max_radius);

	float x_min							= min_radius + radius;
	float x_max							= x_min + cross_length;

	float y_min							= x_min;
	float y_max							= x_max;

	// 0
	pv->set					(center.x+1,center.y + y_min, cross_color); pv++;
	pv->set					(center.x+1,center.y + y_max, cross_color); pv++;
	// 1
	pv->set					(center.x+1,center.y - y_min , cross_color); pv++;
	pv->set					(center.x+1,center.y - y_max, cross_color); pv++;
	// 2
	pv->set					(center.x + x_min+1, center.y, cross_color); pv++;
	pv->set					(center.x + x_max+1, center.y, cross_color); pv++;
	// 3
	pv->set					(center.x - x_min, center.y, cross_color); pv++;
	pv->set					(center.x - x_max, center.y, cross_color); pv++;
	// 4
	pv->set					(center.x, center.y, cross_color); pv++;
	pv->set					(center.x+1, center.y, cross_color); pv++;
//*/
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hGeomLine->vb_stride);

	RCache.set_Shader		(hShader);
	RCache.set_Geometry		(hGeomLine);
	RCache.Render	   		(D3DPT_LINELIST,dwOffset,dwCount/2);


	if(!fsimilar(target_radius,radius))
	{
		float sp				= radius_speed_perc * float(Device.dwWidth) ;
		float radius_change		= sp*Device.fTimeDelta;
		clamp					(radius_change, 0.0f, sp*0.033f); // clamp to 30 fps
		clamp					(radius_change, 0.0f, _abs(target_radius-radius));

		if(target_radius < radius)
			radius -= radius_change;
		else
			radius += radius_change;
	};
/*
	if(_abs(target_radius - radius)>0){
		if(target_radius < radius)
			radius -= radius_speed;
		else
			radius += radius_speed;
	};

	clamp(radius,2.0f,radius);
*/
}