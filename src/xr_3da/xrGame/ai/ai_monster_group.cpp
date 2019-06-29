#include "stdafx.h"
#include "ai_monster_defs.h"
#include "ai_monster_group.h"
#include "../entity_alive.h"
#include "../movement_manager.h"

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

//////////////////////////////////////////////////////////////////////////
// MONSTER SQUAD Implementation
//////////////////////////////////////////////////////////////////////////


void CMonsterSquad::RegisterMember(CEntity *pE)
{
	SEntityState S;
	S.pEnemy = 0;
	states.insert(mk_pair(pE, S));

	if (!leader) leader = pE;
}

void CMonsterSquad::RemoveMember(CEntity *pE)
{
	ENTITY_STATE_MAP_IT it = states.find(pE);
	if (it == states.end()) return;

	states.erase(it);

	if (leader == pE)  {
		if (states.empty()) leader = 0;
		else leader = states.begin()->first;
	}
}

//////////////////////////////////////////////////////////////////////////
// Decentralized Approach (just general data for monsters)
//////////////////////////////////////////////////////////////////////////

void CMonsterSquad::UpdateMonsterData(CEntity *pE, CEntity *pEnemy)
{
	SEntityState	S;
	S.pEnemy		= pEnemy;
	S.last_updated	= Level().timeServer();
	
	ENTITY_STATE_MAP_IT it = states.find(pE);
	if (it == states.end()) {
		S.target.set(0.f,0.f,0.f);
		states.insert(mk_pair(pE, S));
	} else {
		S.target = it->second.target;
		it->second = S;
	}

}


Fvector CMonsterSquad::GetTargetPoint(CEntity *pE, TTime &time)
{
	ENTITY_STATE_MAP_IT it = states.find(pE);
	R_ASSERT(it != states.end());
	
	time = it->second.last_repos;
	return it->second.target;
}


struct remove_predicate {
	CEntity *pO;
	
			remove_predicate	(CEntity *o)	{pO = o;}
	bool	operator()			(SMemberEnemy &e)	{return (e.enemy == pO);}
};



void CMonsterSquad::UpdateDecentralized()
{
	vect_copy.reserve(states.size());
	for (ENTITY_STATE_MAP_IT i = states.begin(); i != states.end(); i++) {
		
		// не учитывать мертвых врагов
		if (!i->first->g_Alive()) continue;

		SMemberEnemy me;
		me.member	= i->first;
		me.enemy	= i->second.pEnemy;
		vect_copy.push_back(me);
	}
	
	general_enemy.reserve(vect_copy.size());
	
	while (!vect_copy.empty()) {
		MEMBER_ENEMY_VEC_IT it	= vect_copy.begin();
		CEntity	*pGeneralEnemy	= it->enemy;

		// заполнить general_enemy
		for (;it != vect_copy.end(); it++) {
			if (it->enemy == pGeneralEnemy) {
				// проверить, являются ли данные актуальными
				general_enemy.push_back(*it);
			}
		}

		if (pGeneralEnemy != 0) {
			SetupMemeberPositions_TargetDir(general_enemy,pGeneralEnemy);
		}

		//VERIFY(general_enemy.empty());
		
		// удалить все элементы из temp_v, у которых враг = pGeneralEnemy
		MEMBER_ENEMY_VEC_IT iter = std::remove_if(vect_copy.begin(), vect_copy.end(), remove_predicate(pGeneralEnemy));
		vect_copy.erase(iter, vect_copy.end());
	}

	vect_copy.clear		();
	general_enemy.clear	();
}

struct sort_predicate {
			CEntity *enemy;

			sort_predicate	(CEntity *pEnemy) : enemy(pEnemy) {}

	bool	operator()		(SMemberEnemy &pE1, SMemberEnemy &pE2) const
	{
		return	(pE1.member->Position().distance_to(enemy->Position()) > 
				 pE2.member->Position().distance_to(enemy->Position()));
	};
};


struct _line {
	CEntity *pE;
	Fvector p_from;
	Fvector p_to;
	bool	b_changed;
};

#define MIN_ANGLE						PI_DIV_6
#define MAX_DIST_THRESHOLD				10.f
#define CHANGE_DIR_ANGLE				PI_DIV_4
#define MIN_DIST_TO_SKIP_ANGLE_CHECK	2.5f
#define ENABLE_RANDOM_SIDE	

void CMonsterSquad::SetupMemeberPositions_TargetDir(MEMBER_ENEMY_VEC &members, CEntity *enemy)
{
	_elem					first;
	_elem					last;

	lines.clear();

	// сортировать по убыванию расстояния от npc до врага 
	std::sort(members.begin(), members.end(), sort_predicate(enemy));

	if (members.empty()) return;

	float delta_yaw = PI_MUL_2 / members.size();

	// обработать ближний элемент
	first.pE		= members.back().member;
	first.p_from	= first.pE->Position();
	first.yaw		= 0;
	members.pop_back();

	lines.push_back(first);

	// обработать дальний элемент
	if (!members.empty()) {
		last.pE			= members[0].member;
		last.p_from		= last.pE->Position();
		last.yaw		= PI;
		members.erase	(members.begin());

		lines.push_back(last);
	}

	Fvector target_pos = enemy->Position();
	float	next_right_yaw	= delta_yaw;
	float	next_left_yaw	= delta_yaw;

	// проходим с конца members в начало (начиная с наименьшего расстояния)
	while (!members.empty()) {
		CEntity *pCur;

		pCur = members.back().member;
		members.pop_back();
		
		_elem cur_line;
		cur_line.p_from		= pCur->Position();
		cur_line.pE			= pCur;

		// определить cur_line.yaw
		
		float h1,p1,h2,p2;
		Fvector dir;
		dir.sub(target_pos, first.p_from);
		dir.getHP(h1,p1);	
		dir.sub(target_pos, cur_line.p_from);
		dir.getHP(h2,p2);
	
		bool b_add_left = false;
		
		if (angle_normalize_signed(h2 - h1) > 0)  {		// right
			if ((next_right_yaw < PI) && !fsimilar(next_right_yaw, PI, PI/60.f)) b_add_left = false;
			else b_add_left = true;
		} else {										// left
			if ((next_left_yaw < PI) && !fsimilar(next_left_yaw, PI, PI/60.f)) b_add_left = true;
			else b_add_left = false;
		}

		if (b_add_left) {
			cur_line.yaw = -next_left_yaw;
			next_left_yaw += delta_yaw;
		} else {
			cur_line.yaw = next_right_yaw;
			next_right_yaw += delta_yaw;
		}

		lines.push_back(cur_line);
	}


	// Пройти по всем линиям и заполнить таргеты у npc
	float first_h, first_p;
	Fvector d; d.sub(target_pos,first.p_from);
	d.getHP(first_h, first_p);

	for (u32 i = 0; i < lines.size(); i++){
		ENTITY_STATE_MAP_IT	it	= states.find(lines[i].pE);
		R_ASSERT(it != states.end());
		
		float h = first_h + lines[i].yaw;
		it->second.target.setHP(angle_normalize(h),first_p);
	}
}

void CMonsterSquad::SetupMemeberPositions_Deviation(MEMBER_ENEMY_VEC &members, CEntity *enemy)
{
	xr_vector<_line> lines;
	lines.reserve(members.size());

	// сортировать по убыванию расстояния от npc до врага 
	std::sort(members.begin(), members.end(), sort_predicate(enemy));

	// проходим с конца members в начало (начиная с наименьшего расстояния)
	while (!members.empty()) {
		CEntity *pCur;

		pCur = members.back().member;
		members.pop_back();
		
		// построить текущую линию
		_line cur_line;
		cur_line.p_from		= pCur->Position();
		cur_line.p_to		= enemy->Position();
		cur_line.pE			= pCur;
		cur_line.b_changed	= false;
		
		// Установить линии атаки

		// Пройти по всем линиям и определить линию, с которой минимальный угол
		float	smallest_angle	= flt_max;
		u32		line_index	= u32(-1);
		float	min_dist		= flt_max;		// дистанция до ближайшего NPC

		for (u32 i = 0; i < lines.size(); i++){
			Fvector dir1, dir2;
			dir1.sub(lines[i].p_to,		lines[i].p_from); 
			dir2.sub(cur_line.p_to,		cur_line.p_from);
			
			dir1.normalize();
			dir2.normalize();

			// определить угол между 2-мя векторами
			float angle = acosf(dir1.dotproduct(dir2));
			
			if ((angle > MIN_ANGLE) || (lines[i].b_changed)) continue;
			else {
				if (angle < smallest_angle) {
					smallest_angle	= angle;
					line_index		= i;
				}
			}
			
			float cur_dist = cur_line.p_from.distance_to(lines[i].p_from);
			if (cur_dist < min_dist) min_dist = cur_dist;
		}


		// установить target_pos
		if ((line_index == u32(-1)) || (!fsimilar(min_dist,flt_max) && (min_dist > MIN_DIST_TO_SKIP_ANGLE_CHECK))) {	// нет слишком малых углов
			lines.push_back(cur_line);
		} else {
			_line nearest = lines[line_index];
			
			// определить сторону на какой находится nearest.p_from
			Fvector cur_dir, dir2;
			float	h, p, h2, p2;
			
			cur_dir.sub(cur_line.p_to,cur_line.p_from);
			cur_dir.getHP(h,p);
			dir2.sub(nearest.p_from,cur_line.p_from);
			dir2.getHP(h2,p2);

			float d_angle;
			d_angle = ((angle_normalize_signed(h2 - h) > 0) ? -CHANGE_DIR_ANGLE : CHANGE_DIR_ANGLE);

#ifdef ENABLE_RANDOM_SIDE
			if (::Random.randI(100) < 50) { 
				if (::Random.randI(2)) d_angle = -CHANGE_DIR_ANGLE;
				else d_angle = CHANGE_DIR_ANGLE;
			}
#endif

			float dist;
			dist = cur_dir.magnitude();
			if (dist > MAX_DIST_THRESHOLD) dist = MAX_DIST_THRESHOLD;

			cur_dir.setHP(h + d_angle,p);
			cur_dir.normalize();

			Fvector new_pos;
			new_pos.mad(cur_line.p_from, cur_dir, dist);

			cur_line.p_to		= new_pos;
			cur_line.b_changed	= true;
			lines.push_back(cur_line);
		}
	}
	
	// Пройти по всем линиям и заполнить таргеты у npc
	TTime cur_time = Level().timeServer();

	for (u32 i = 0; i < lines.size(); i++){
		ENTITY_STATE_MAP_IT	it	= states.find(lines[i].pE);
		R_ASSERT(it != states.end());
			
		it->second.target	= lines[i].p_to;
		
		if (lines[i].b_changed) it->second.last_repos = cur_time;
		else it->second.last_repos = 0;

	}
}

bool CMonsterSquad::SquadActive()
{
	if (!leader || (states.size() <= 1)) return false;

	u32 alive_num = 0;
	for (ENTITY_STATE_MAP_IT it = states.begin(); it != states.end(); it++) 
		if (it->first->g_Alive()) alive_num++;

	if (alive_num <= 1) return false;

	return true;	
}

