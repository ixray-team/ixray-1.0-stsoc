////////////////////////////////////////////////////////////////////////////
//	Module 		: squad_hierarchy_holder.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Squad hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "seniority_hierarchy_space.h"

class CGroupHierarchyHolder;
class CEntity;
class CTeamHierarchyHolder;

namespace SquadHierarchyHolder {
	typedef xr_vector<CGroupHierarchyHolder*>		GROUP_REGISTRY;
}

class CSquadHierarchyHolder {
private:
	enum {max_group_count = 32};

private:
	typedef SquadHierarchyHolder::GROUP_REGISTRY	GROUP_REGISTRY;

private:
	CTeamHierarchyHolder			*m_team;
	CEntity							*m_leader;
	mutable GROUP_REGISTRY			m_groups;

public:
	IC								CSquadHierarchyHolder	(CTeamHierarchyHolder *team);
	virtual							~CSquadHierarchyHolder	();
			CGroupHierarchyHolder	&group					(u32 group_id) const;
	IC		CEntity					*leader					() const;
	IC		CTeamHierarchyHolder	&team					() const;
	IC		void					leader					(CEntity *leader);
			void					update_leader			();
	IC		const GROUP_REGISTRY	&groups					() const;
};

#include "squad_hierarchy_holder_inline.h"