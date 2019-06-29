////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_patrol_path_manager_inline.h
//	Created 	: 01.11.2005
//  Modified 	: 01.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human patrol path manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeHumanPatrolPathManager::object_type &CALifeHumanPatrolPathManager::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

IC	void CALifeHumanPatrolPathManager::path					(const CPatrolPath *path)
{
	m_actual				= m_actual && (m_path == path);
	m_path					= path;
}

IC	void CALifeHumanPatrolPathManager::path					(LPCSTR path_name)
{
	path					(shared_str(path_name));
}

IC	bool CALifeHumanPatrolPathManager::actual				() const
{
	return					(m_actual);
}

IC	bool CALifeHumanPatrolPathManager::completed			() const
{
	return					(actual() && m_completed);
}

IC	void CALifeHumanPatrolPathManager::start_type			(const EPatrolStartType	&start_type)
{
	m_start_type			= start_type;
}

IC	void CALifeHumanPatrolPathManager::route_type			(const EPatrolRouteType	&route_type)
{
	m_route_type			= route_type;
}

IC	const CALifeHumanPatrolPathManager::EPatrolStartType &CALifeHumanPatrolPathManager::start_type	() const
{
	return					(m_start_type);
}

IC	const CALifeHumanPatrolPathManager::EPatrolRouteType &CALifeHumanPatrolPathManager::route_type	() const
{
	return					(m_route_type);
}

IC	const CPatrolPath &CALifeHumanPatrolPathManager::path	() const
{
	VERIFY					(m_path);
	return					(*m_path);
}

IC	void CALifeHumanPatrolPathManager::start_vertex_index	(const u32 &start_vertex_index)
{
	m_start_vertex_index	= start_vertex_index;
}

IC	bool CALifeHumanPatrolPathManager::use_randomness		() const
{
	return					(m_use_randomness);
}

IC	void CALifeHumanPatrolPathManager::use_randomness		(const bool &use_randomness)
{
	m_use_randomness		= use_randomness;
}
