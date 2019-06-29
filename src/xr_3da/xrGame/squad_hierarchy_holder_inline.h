////////////////////////////////////////////////////////////////////////////
//	Module 		: squad_hierarchy_holder_inline.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Squad hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSquadHierarchyHolder::CSquadHierarchyHolder									(CTeamHierarchyHolder *team)
{
	VERIFY						(team);
	m_team						= team;
	m_leader					= 0;
	SeniorityHierarchy::assign_svector	(m_groups,max_group_count,0);
}

IC	CEntity	*CSquadHierarchyHolder::leader											() const
{
	return						(m_leader);
}

IC	CTeamHierarchyHolder &CSquadHierarchyHolder::team								() const
{
	VERIFY						(m_team);
	return						(*m_team);
}

IC	void CSquadHierarchyHolder::leader												(CEntity *leader)
{
	m_leader					= leader;
}

IC	const SquadHierarchyHolder::GROUP_REGISTRY &CSquadHierarchyHolder::groups		() const
{
	return						(m_groups);
}
