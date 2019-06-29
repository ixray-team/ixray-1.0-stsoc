#include "stdafx.h"
#include "flightscontroller.h"
#include "..\xrLevel.h"

void CLightDB_Static::Load			(IReader *fs) 
{
	IReader* F	= 0;

	// Lights itself
	LDirect		= NULL;
	{
		F				= fs->open_chunk		(fsL_LIGHT_DYNAMIC);

		u32 size		= F->length();
		u32 element		= sizeof(Flight)+4;
		u32 count		= size/element;
		R_ASSERT		(count*element == size);
		Lights.resize	(count);

		for (u32 i=0; i<count; i++) 
		{
			R1_light*	L				= xr_new<R1_light>	();
			Flight&		Ldata			= L->data;
			Lights[i]					= L;
			u32			dummycontrol	= 0;
			F->r						(&dummycontrol,4);
			F->r						(&Ldata,sizeof(Flight));
			L->ID						= i;

			Ldata.specular.set			(Ldata.diffuse);
			Ldata.specular.mul_rgb		(0.2f);
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)
			{
				Ldata.position.invert	(Ldata.direction);
				Ldata.position.mul		(1000.f);
				Ldata.range				= 1000.f;
				LDirect					= L;
			}

			L->spatial.type				= STYPE_LIGHTSOURCE;
			L->spatial.center			= Ldata.position;
			L->spatial.radius			= Ldata.range;
			if (Ldata.type!=D3DLIGHT_DIRECTIONAL)	L->spatial_register	();
		}

		F->close		();
	}
	R_ASSERT2			(LDirect,"Where is directional light???");
}

void CLightDB_Static::Unload		(void)
{
	for (u32 i=0; i<Lights.size(); i++) 
	{
		Lights[i]->spatial_unregister	();
		xr_delete						(Lights[i]);
	}
	Lights.clear						();
}

IC float spline(float *m, float p0, float p1, float p2, float p3)
{	return p0 * m[0] + p1 * m[1] + p2 * m[2] + p3 * m[3]; }

IC void t_spline(float t, float *m) 
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );
}
