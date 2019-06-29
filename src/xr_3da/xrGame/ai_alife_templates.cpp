////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_templates.cpp
//	Created 	: 21.01.2003
//  Modified 	: 03.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life templates
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_templates.h"

void delete_data(LPSTR &tpData)
{
	xr_free					(tpData);
}

void delete_data(LPCSTR &tpData)
{
}

