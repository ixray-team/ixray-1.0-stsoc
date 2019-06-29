#include "stdafx.h"
#include "monster_enemy_manager.h"
#include "biting/ai_biting.h"
#include "ai_monsters_misc.h"


CMonsterEnemyManager::CMonsterEnemyManager()
{
	monster		= 0;
	enemy		= 0;
	flags.zero	();
	forced		= false;
	prev_enemy	= 0;
	danger_type = eNone;
}

CMonsterEnemyManager::~CMonsterEnemyManager()
{

}
void CMonsterEnemyManager::init_external(CAI_Biting *M)
{
	monster = M;
}


void CMonsterEnemyManager::update()
{
	if (forced) {
		// проверить валидность force-объекта
		if (!enemy || enemy->getDestroy() || !enemy->g_Alive()) {
			enemy = 0;
			return;
		}
	} else {
		enemy = monster->EnemyMemory.get_enemy();
		
		if (enemy) {
			SMonsterEnemy enemy_info	= monster->EnemyMemory.get_enemy_info();
			position					= enemy_info.position;
			vertex						= enemy_info.vertex;
			time_last_seen				= enemy_info.time;
		}
	}
	
	if (!enemy) {
		return;
	}
	
	// проверить видимость
	enemy_see_me = false;
	if (monster->CVisualMemoryManager::visible_now(enemy)) enemy_see_me = true;
	
	// обновить опасность врага
	danger_type = eNone;

	switch (dwfChooseAction(0, 0.8f, 0.6f, 0.4f, 0.2f,monster->g_Team(),monster->g_Squad(),monster->g_Group(),0,1,2,3,4, monster, 30.f)) {
		case 4 : 	danger_type = eVeryStrong;	break;
		case 3 : 	danger_type = eStrong;		break;
		case 2 : 	danger_type = eNormal;		break;
		case 1 : 
		case 0 : 	danger_type = eWeak;		break;
	}

	// обновить флаги
	flags.zero();

	if ((prev_enemy == enemy) && (time_last_seen != Level().timeServer()))	flags.or(FLAG_ENEMY_LOST_SIGHT);		
	if (prev_enemy && !prev_enemy->g_Alive())								flags.or(FLAG_ENEMY_DIE);
	if (!enemy_see_me)														flags.or(FLAG_ENEMY_DOESNT_SEE_ME);
	
	float dist_now, dist_prev;
	if (prev_enemy == enemy) {
		dist_now	= position.distance_to(monster->Position());
		dist_prev	= prev_enemy_position.distance_to(monster->Position());

		if (_abs(dist_now - dist_prev) < 0.2f)								flags.or(FLAG_ENEMY_STANDING);
		else {
			if (dist_now < dist_prev)										flags.or(FLAG_ENEMY_GO_CLOSER);
			else															flags.or(FLAG_ENEMY_GO_FARTHER);

			if (_abs(dist_now - dist_prev) < 1.2f) {
				if (dist_now < dist_prev)									flags.or(FLAG_ENEMY_GO_CLOSER_FAST);
				else														flags.or(FLAG_ENEMY_GO_FARTHER_FAST);
			}
		}

		if (flags.is(FLAG_ENEMY_STANDING) && flags.is(FLAG_ENEMY_DOESNT_SEE_ME)) flags.or(FLAG_ENEMY_DOESNT_KNOW_ABOUT_ME);
	}

	// сохранить текущего врага
	prev_enemy			= enemy;
	prev_enemy_position = position;

	expediency			= true;
}



void CMonsterEnemyManager::force_enemy (const CEntityAlive *enemy)
{
	this->enemy		= enemy;
	position		= enemy->Position();
	vertex			= enemy->level_vertex_id();
	time_last_seen	= Level().timeServer();

	forced			= true;
}

void CMonsterEnemyManager::unforce_enemy()
{
	enemy = monster->EnemyMemory.get_enemy();

	if (enemy) {
		SMonsterEnemy enemy_info	= monster->EnemyMemory.get_enemy_info();
		position					= enemy_info.position;
		vertex						= enemy_info.vertex;
		time_last_seen				= enemy_info.time;
	}

	forced = false;
}


u32	CMonsterEnemyManager::get_enemies_count()
{
	return monster->EnemyMemory.get_enemies_count();
}

void CMonsterEnemyManager::reinit()
{
	enemy			= 0;
	time_last_seen	= 0;
	flags.zero		();
	forced			= false;
	prev_enemy		= 0;
	danger_type		= eNone;
}


void CMonsterEnemyManager::add_enemy(const CEntityAlive *enemy)
{
	monster->EnemyMemory.add_enemy(enemy);
}


bool CMonsterEnemyManager::see_enemy_now()
{
	if (time_last_seen == monster->m_current_update) return true;
	return false;
}
