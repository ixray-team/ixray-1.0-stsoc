////////////////////////////////////////////////////////////////////////////
//	Module 		: object_broker.h
//	Created 	: 21.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object broker
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_broker.h"

void delete_data(LPSTR &tpData)
{
	xr_free					(tpData);
}

void delete_data(LPCSTR &tpData)
{
}
