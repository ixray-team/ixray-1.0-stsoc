#include "stdafx.h"

u16		xrServer::PerformIDgen		(u16 ID)
{
	if (0xffff==ID)		
	{
		// GENERATE
		R_ASSERT	(!id_free.empty());
		ID			= id_free.front();
		id_free.pop_front			();
		return		ID;
	} else {
		// USE SUPPLIED
		for (xr_deque<u16>::iterator I=id_free.begin(); I!=id_free.end(); I++)
		{
			if (*I==ID)	
			{
				id_free.erase	(I);
				return ID;
			}
		}

		Debug.fatal		("Requesting ID already used.");
		return		0xffff;
	}
}
