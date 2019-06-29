////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory_inline.h
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CVisualMemoryManager &CActorMemory::visual	() const
{
	VERIFY		(m_visual);
	return		(*m_visual);
}

IC	CActor &CActorMemory::object				() const
{
	VERIFY		(m_actor);
	return		(*m_actor);
}
