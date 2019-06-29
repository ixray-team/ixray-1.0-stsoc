#include "stdafx.h"
#include "ai_monster_squad_manager.h"
#include "ai_monster_squad.h"
#include "../entity.h"

//////////////////////////////////////////////////////////////////////////
// SQUAD MANAGER Implementation
//////////////////////////////////////////////////////////////////////////
CMonsterSquadManager *g_monster_squad = 0;

CMonsterSquadManager::CMonsterSquadManager()
{
}
CMonsterSquadManager::~CMonsterSquadManager()
{
	for (u32 team_id=0; team_id<team.size();team_id++) {
		for (u32 squad_id=0; squad_id<team[team_id].size(); squad_id++) {
			xr_delete(team[team_id][squad_id]);
		}
	}
}

void CMonsterSquadManager::register_member(u8 team_id, u8 squad_id, CEntity *e)
{
	CMonsterSquad *pSquad;

	// нет team - создать team и squad
	if (team_id >= team.size()) { 
		team.resize(team_id + 1);

		team[team_id].resize(squad_id+1);

		if (squad_id > 0) {
			for (u32 i=0; i<squad_id; i++) {
				team[team_id][i] = 0;
			}
		}

		pSquad = xr_new<CMonsterSquad>();
		team[team_id][squad_id]	= pSquad;

		// есть team, нет squad - создать squad
	} else if (squad_id >= team[team_id].size()) { 
		u32 prev_size = team[team_id].size();
		team[team_id].resize(squad_id + 1);

		for (u32 i = prev_size; i < squad_id; i++) {
			team[team_id][i] = 0;
		}
		pSquad = xr_new<CMonsterSquad>();
		team[team_id][squad_id]	= pSquad;
	} else {
		if (team[team_id][squad_id] == 0) {
			pSquad = xr_new<CMonsterSquad>();
			team[team_id][squad_id]	= pSquad;
		} else pSquad = team[team_id][squad_id];
	}

	pSquad->RegisterMember(e);
}

void CMonsterSquadManager::remove_member(u8 team_id, u8 squad_id, CEntity *e)
{
	get_squad(team_id, squad_id)->RemoveMember(e);
}

CMonsterSquad *CMonsterSquadManager::get_squad(u8 team_id, u8 squad_id)
{
	VERIFY((team_id < team.size()) && (squad_id < team[team_id].size()));
	return team[team_id][squad_id];
}

CMonsterSquad *CMonsterSquadManager::get_squad(const CEntity *entity)
{
	return get_squad((u8)entity->g_Team(),(u8)entity->g_Squad());
}

void CMonsterSquadManager::update(CEntity *entity)
{
	CMonsterSquad	*squad = monster_squad().get_squad(entity);
	if (squad && squad->SquadActive() && (squad->GetLeader() == entity)) {
		squad->UpdateSquadCommands();
	}
}

