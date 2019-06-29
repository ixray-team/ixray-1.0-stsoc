////////////////////////////////////////////////////////////////////////////
//	Module 		: squad.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Squad class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "group.h"
#include "memory_space.h"

const int maxGroups				= 32;

class CSquad {
public:
	CEntity						*Leader;
	xr_vector<CGroup>			Groups;
	xr_vector<MemorySpace::CVisibleObject>	m_visible_objects;
	xr_vector<MemorySpace::CSoundObject>	m_sound_objects;
	xr_vector<MemorySpace::CHitObject>		m_hit_objects;
	u32							m_member_count;

								CSquad() : 
									Leader(0),
									m_member_count(0)
	{
	}

								~CSquad()
	{
	}

	bool						Empty			(){return Groups.empty();}
	int							Size			(){return Groups.size();}
};
