#include "stdafx.h"
#include "Level.h"
#include "xrServer.h"

void	CCC_SV_Int::Execute	(LPCSTR args)
{
	if (strstr(args, "#set_"))
	{			  
		int* pNewValue = NULL;
		sscanf(args+5, "%x", &pNewValue);
		if (NULL == pNewValue)
		{
			value = &StoredValue;
		}
		else
		{
			*pNewValue = StoredValue;
			value = pNewValue;
		};			  
		return;
	};

	CCC_Integer::Execute(args);
	StoredValue = *value;
	Level().Server->game->signal_Syncronize();	  
}


void	CCC_SV_Flt::Execute	(LPCSTR args)
{
	if (strstr(args, "#set_"))
	{			  
		float* pNewValue = NULL;
		sscanf(args+5, "%x", &pNewValue);
		if (NULL == pNewValue)
		{
			value = &StoredValue;
		}
		else
		{
			*pNewValue = StoredValue;
			value = pNewValue;
		};			  
		return;
	};

	CCC_Float::Execute(args);
	StoredValue = *value;
	Level().Server->game->signal_Syncronize();
}
