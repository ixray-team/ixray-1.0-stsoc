// Spawn.cpp: implementation of the CSpawn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Spawn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpawn::CSpawn		()
{

}

CSpawn::~CSpawn		()
{

}

void CSpawn::Save	(CFS_Base& FS)
{
	FS.write		(&description,sizeof(description));
}
void CSpawn::Load	(CStream& FS)
{
	FS.Read			(&description,sizeof(description));
}

#include "spawn_event.h"
CSpawn* CSpawn::Create		(CLASS_ID cls)
{
	switch (cls)
	{
	case CLSID_SPAWN_EVENT:		return new CSpawn_Event;
	case CLSID_SPAWN_ZONE:		return 0;
	case CLSID_SPAWN_ENTITY:	return 0;
	default:					return 0;
	}
}

void CSpawn::CreatePalette	(vector<CSpawn*> & palette)
{
	palette.push_back		(Create(CLSID_SPAWN_EVENT));
	palette.push_back		(Create(CLSID_SPAWN_ZONE));
	palette.push_back		(Create(CLSID_SPAWN_ENTITY));
}
