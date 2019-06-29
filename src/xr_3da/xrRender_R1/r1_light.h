#pragma	 once

#include "..\_d3d_extensions.h"
#include "..\ispatial.h"

#pragma pack(push,4)
class R1_light		: public ISpatial
{
public:
	Flight			data;
	u32				ID;
public:
};
#pragma pack(pop)
