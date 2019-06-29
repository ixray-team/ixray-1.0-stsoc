////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_movement_manager_inline.h
//	Created 	: 31.10.2005
//  Modified 	: 31.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human movement manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeHumanMovementManager::object_type &CALifeHumanMovementManager::object			() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

IC	CALifeHumanMovementManager::detail_path_type &CALifeHumanMovementManager::detail	() const
{
	VERIFY		(m_detail);
	return		(*m_detail);
}

IC	CALifeHumanMovementManager::patrol_path_type &CALifeHumanMovementManager::patrol	() const
{
	VERIFY		(m_patrol);
	return		(*m_patrol);
}

IC	const CALifeHumanMovementManager::EPathType &CALifeHumanMovementManager::path_type	() const
{
	return		(m_path_type);
}

IC	void CALifeHumanMovementManager::path_type	(const EPathType &path_type)
{
	m_path_type	= path_type;
}
