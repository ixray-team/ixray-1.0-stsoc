#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../entity.h"
#include "../level.h"

void CMonsterSquad::ProcessAttack()
{
	m_enemy_map.clear		();
	m_temp_entities.clear	();

	// Выделить элементы с общими врагами и состянием атаки 
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); it_goal++) {
		SMemberGoal goal = it_goal->second;

		if (goal.type == MG_AttackEnemy) {
			VERIFY(goal.entity);

			ENEMY_MAP_IT it = m_enemy_map.find(goal.entity);
			if (it != m_enemy_map.end()) {
				it->second.push_back(it_goal->first);
			} else {
				m_temp_entities.push_back	(it_goal->first);
				m_enemy_map.insert			(mk_pair(goal.entity, m_temp_entities));
			}
		}
	}

	// Пройти по всем группам и назначить углы всем елементам в группе
	for (ENEMY_MAP_IT it_enemy = m_enemy_map.begin(); it_enemy != m_enemy_map.end(); it_enemy++) {
		Attack_AssignTargetDir(it_enemy->second,it_enemy->first);
	}
}


struct sort_predicate {
	CEntity *enemy;

			
			sort_predicate	(CEntity *pEnemy) : enemy(pEnemy) {}

	bool	operator()		(CEntity *pE1, CEntity *pE2) const
	{
		return	(pE1->Position().distance_to(enemy->Position()) > 
			pE2->Position().distance_to(enemy->Position()));
	};
};

void CMonsterSquad::Attack_AssignTargetDir(ENTITY_VEC &members, CEntity *enemy)
{
	_elem					first;
	_elem					last;

	lines.clear();

	// сортировать по убыванию расстояния от npc до врага 
	std::sort(members.begin(), members.end(), sort_predicate(enemy));
	if (members.empty()) return;

	float delta_yaw = PI_MUL_2 / members.size();

	// обработать ближний элемент
	first.pE		= members.back();
	first.p_from	= first.pE->Position();
	first.yaw		= 0;
	members.pop_back();

	lines.push_back(first);

	// обработать дальний элемент
	if (!members.empty()) {
		last.pE			= members[0];
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

		pCur = members.back();
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
		SSquadCommand command;
		command.type			= SC_ATTACK;
		command.entity			= enemy;
		command.direction.setHP	(angle_normalize(first_h + lines[i].yaw), first_p);
		UpdateCommand(lines[i].pE, command);
	}
}
















//struct sort_predicate {
//	CEntity *enemy;
//
//	sort_predicate	(CEntity *pEnemy) : enemy(pEnemy) {}
//
//	bool	operator()		(CMonsterSquad::SMemberEnemy &pE1, CMonsterSquad::SMemberEnemy &pE2) const
//	{
//		return	(pE1.member->Position().distance_to(enemy->Position()) > 
//			pE2.member->Position().distance_to(enemy->Position()));
//	};
//};
//
//
//struct _line {
//	CEntity *pE;
//	Fvector p_from;
//	Fvector p_to;
//	bool	b_changed;
//};
//
//#define MIN_ANGLE						PI_DIV_6
//#define MAX_DIST_THRESHOLD				10.f
//#define CHANGE_DIR_ANGLE				PI_DIV_4
//#define MIN_DIST_TO_SKIP_ANGLE_CHECK	2.5f
//#define ENABLE_RANDOM_SIDE	
//
//void CMonsterSquad::SetupMemeberPositions_TargetDir(MEMBER_ENEMY_VEC &members, CEntity *enemy)
//{
//	_elem					first;
//	_elem					last;
//
//	lines.clear();
//
//	// сортировать по убыванию расстояния от npc до врага 
//	std::sort(members.begin(), members.end(), sort_predicate(enemy));
//
//	if (members.empty()) return;
//
//	float delta_yaw = PI_MUL_2 / members.size();
//
//	// обработать ближний элемент
//	first.pE		= members.back().member;
//	first.p_from	= first.pE->Position();
//	first.yaw		= 0;
//	members.pop_back();
//
//	lines.push_back(first);
//
//	// обработать дальний элемент
//	if (!members.empty()) {
//		last.pE			= members[0].member;
//		last.p_from		= last.pE->Position();
//		last.yaw		= PI;
//		members.erase	(members.begin());
//
//		lines.push_back(last);
//	}
//
//	Fvector target_pos = enemy->Position();
//	float	next_right_yaw	= delta_yaw;
//	float	next_left_yaw	= delta_yaw;
//
//	// проходим с конца members в начало (начиная с наименьшего расстояния)
//	while (!members.empty()) {
//		CEntity *pCur;
//
//		pCur = members.back().member;
//		members.pop_back();
//
//		_elem cur_line;
//		cur_line.p_from		= pCur->Position();
//		cur_line.pE			= pCur;
//
//		// определить cur_line.yaw
//
//		float h1,p1,h2,p2;
//		Fvector dir;
//		dir.sub(target_pos, first.p_from);
//		dir.getHP(h1,p1);	
//		dir.sub(target_pos, cur_line.p_from);
//		dir.getHP(h2,p2);
//
//		bool b_add_left = false;
//
//		if (angle_normalize_signed(h2 - h1) > 0)  {		// right
//			if ((next_right_yaw < PI) && !fsimilar(next_right_yaw, PI, PI/60.f)) b_add_left = false;
//			else b_add_left = true;
//		} else {										// left
//			if ((next_left_yaw < PI) && !fsimilar(next_left_yaw, PI, PI/60.f)) b_add_left = true;
//			else b_add_left = false;
//		}
//
//		if (b_add_left) {
//			cur_line.yaw = -next_left_yaw;
//			next_left_yaw += delta_yaw;
//		} else {
//			cur_line.yaw = next_right_yaw;
//			next_right_yaw += delta_yaw;
//		}
//
//		lines.push_back(cur_line);
//	}
//
//
//	// Пройти по всем линиям и заполнить таргеты у npc
//	float first_h, first_p;
//	Fvector d; d.sub(target_pos,first.p_from);
//	d.getHP(first_h, first_p);
//
//	for (u32 i = 0; i < lines.size(); i++){
//		ENTITY_STATE_MAP_IT	it	= states.find(lines[i].pE);
//		R_ASSERT(it != states.end());
//
//		float h = first_h + lines[i].yaw;
//		it->second.target.setHP(angle_normalize(h),first_p);
//	}
//}
//
//void CMonsterSquad::SetupMemeberPositions_Deviation(MEMBER_ENEMY_VEC &members, CEntity *enemy)
//{
//	xr_vector<_line> lines;
//	lines.reserve(members.size());
//
//	// сортировать по убыванию расстояния от npc до врага 
//	std::sort(members.begin(), members.end(), sort_predicate(enemy));
//
//	// проходим с конца members в начало (начиная с наименьшего расстояния)
//	while (!members.empty()) {
//		CEntity *pCur;
//
//		pCur = members.back().member;
//		members.pop_back();
//
//		// построить текущую линию
//		_line cur_line;
//		cur_line.p_from		= pCur->Position();
//		cur_line.p_to		= enemy->Position();
//		cur_line.pE			= pCur;
//		cur_line.b_changed	= false;
//
//		// Установить линии атаки
//
//		// Пройти по всем линиям и определить линию, с которой минимальный угол
//		float	smallest_angle	= flt_max;
//		u32		line_index	= u32(-1);
//		float	min_dist		= flt_max;		// дистанция до ближайшего NPC
//
//		for (u32 i = 0; i < lines.size(); i++){
//			Fvector dir1, dir2;
//			dir1.sub(lines[i].p_to,		lines[i].p_from); 
//			dir2.sub(cur_line.p_to,		cur_line.p_from);
//
//			dir1.normalize();
//			dir2.normalize();
//
//			// определить угол между 2-мя векторами
//			float angle = acosf(dir1.dotproduct(dir2));
//
//			if ((angle > MIN_ANGLE) || (lines[i].b_changed)) continue;
//			else {
//				if (angle < smallest_angle) {
//					smallest_angle	= angle;
//					line_index		= i;
//				}
//			}
//
//			float cur_dist = cur_line.p_from.distance_to(lines[i].p_from);
//			if (cur_dist < min_dist) min_dist = cur_dist;
//		}
//
//
//		// установить target_pos
//		if ((line_index == u32(-1)) || (!fsimilar(min_dist,flt_max) && (min_dist > MIN_DIST_TO_SKIP_ANGLE_CHECK))) {	// нет слишком малых углов
//			lines.push_back(cur_line);
//		} else {
//			_line nearest = lines[line_index];
//
//			// определить сторону на какой находится nearest.p_from
//			Fvector cur_dir, dir2;
//			float	h, p, h2, p2;
//
//			cur_dir.sub(cur_line.p_to,cur_line.p_from);
//			cur_dir.getHP(h,p);
//			dir2.sub(nearest.p_from,cur_line.p_from);
//			dir2.getHP(h2,p2);
//
//			float d_angle;
//			d_angle = ((angle_normalize_signed(h2 - h) > 0) ? -CHANGE_DIR_ANGLE : CHANGE_DIR_ANGLE);
//
//#ifdef ENABLE_RANDOM_SIDE
//			if (::Random.randI(100) < 50) { 
//				if (::Random.randI(2)) d_angle = -CHANGE_DIR_ANGLE;
//				else d_angle = CHANGE_DIR_ANGLE;
//			}
//#endif
//
//			float dist;
//			dist = cur_dir.magnitude();
//			if (dist > MAX_DIST_THRESHOLD) dist = MAX_DIST_THRESHOLD;
//
//			cur_dir.setHP(h + d_angle,p);
//			cur_dir.normalize();
//
//			Fvector new_pos;
//			new_pos.mad(cur_line.p_from, cur_dir, dist);
//
//			cur_line.p_to		= new_pos;
//			cur_line.b_changed	= true;
//			lines.push_back(cur_line);
//		}
//	}
//
//	// Пройти по всем линиям и заполнить таргеты у npc
//	u32 cur_time = Level().timeServer();
//
//	for (u32 i = 0; i < lines.size(); i++){
//		ENTITY_STATE_MAP_IT	it	= states.find(lines[i].pE);
//		R_ASSERT(it != states.end());
//
//		it->second.target	= lines[i].p_to;
//
//		if (lines[i].b_changed) it->second.last_repos = cur_time;
//		else it->second.last_repos = 0;
//
//	}
//}




