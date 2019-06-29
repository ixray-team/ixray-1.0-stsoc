////////////////////////////////////////////////////////////////////////////
//	Module 		: prefetch_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Prefetch manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "prefetch_manager.h"
#include "../xr_object.h"

CPrefetchManager::CPrefetchManager		()
{
}

CPrefetchManager::~CPrefetchManager		()
{
}

void CPrefetchManager::Load				(LPCSTR section)
{
	if (!pSettings->line_exist(section,"texture_prefetch"))
		return;

	CObject							*object = smart_cast<CObject*>(this);
	VERIFY							(object);
	
	LPCSTR							textures = pSettings->r_string(section,"texture_prefetch");
	string256						I;
	for (u32 i=0, n=_GetItemCount(textures); i<n; ++i)
		object->cNameVisual_set		(_GetItem(textures,i,I));
}