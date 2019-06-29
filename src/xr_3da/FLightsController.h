#pragma once

#include "r1_light.h"

// refs
class ENGINE_API CObject;
class CLightTrack;

// t-defs
class CLightDB_Static 
{
	xr_vector<R1_light*>		Lights;			// -- Lights itself
	R1_light*					LDirect;
public:
	void	Track				(IRenderable*	O);

	void	Load				(IReader*		fs);
	void	Unload				(void);
};
