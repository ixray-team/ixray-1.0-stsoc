////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_spawn_merge.cpp
//	Created 	: 29.01.2003
//  Modified 	: 29.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level spawns for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_spawn_constructor.h"

void xrMergeSpawns(LPCSTR name, LPCSTR output, LPCSTR start)
{
	CGameSpawnConstructor			spawn(name,output,start);
}
