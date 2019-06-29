#include "stdafx.h"
#include "monster_vision_memory.h"
#include "biting/ai_biting.h"

CMonsterEnemyMemory::CMonsterEnemyMemory()
{
	monster			= 0;
	time_memory		= 10000; 
}

CMonsterEnemyMemory::~CMonsterEnemyMemory()
{
}

void CMonsterEnemyMemory::init_external(CAI_Biting *M, TTime mem_time) 
{
	monster = M; 
	time_memory = mem_time;
}


void CMonsterEnemyMemory::update() 
{
	// Обновить врагов
	for (xr_set<const CEntityAlive *>::const_iterator I = monster->enemies().begin(); I != monster->enemies().end(); ++I) {
		if (!monster->CMemoryManager::visible(*I)) continue;
		add_enemy(*I);
	}

	// удалить устаревших врагов
	remove_non_actual();

	// обновить опасность 
	for (ENEMIES_MAP_IT it = m_objects.begin(); it != m_objects.end(); it++) {
		it->second.danger = monster->Position().distance_to(it->second.position);
	}
}

void CMonsterEnemyMemory::add_enemy(const CEntityAlive *enemy)
{
	SMonsterEnemy enemy_info;
	enemy_info.position = enemy->Position();
	enemy_info.vertex   = enemy->level_vertex_id();
	enemy_info.time		= Level().timeServer();
	enemy_info.danger	= 0.f;

	ENEMIES_MAP_IT it = m_objects.find(enemy);
	if (it != m_objects.end()) {
		// обновить данные о враге
		it->second = enemy_info;
	} else {
		// добавить врага в список объектов
		m_objects.insert(mk_pair(enemy, enemy_info));
	}
}

void CMonsterEnemyMemory::remove_non_actual() 
{
	TTime cur_time = Level().timeServer();

	// удалить 'старых' врагов и тех, расстояние до которых > 30м и др.
	for (ENEMIES_MAP_IT it = m_objects.begin(), nit; it != m_objects.end(); it = nit)
	{
		nit = it; ++nit;
		// проверить условия удаления
		if (	!it->first					|| 
			!it->first->g_Alive()		|| 
			it->first->getDestroy()		||
			(it->second.time + time_memory < cur_time)
			) 
			m_objects.erase (it);
	}
}

const CEntityAlive *CMonsterEnemyMemory::get_enemy()
{
	ENEMIES_MAP_IT	it = find_best_enemy();
	if (it != m_objects.end()) return it->first;
	return (0);
}

SMonsterEnemy CMonsterEnemyMemory::get_enemy_info()
{
	SMonsterEnemy ret_val;
	ret_val.time = 0;

	ENEMIES_MAP_IT	it = find_best_enemy();
	if (it != m_objects.end()) ret_val = it->second;

	return ret_val;
}

ENEMIES_MAP_IT CMonsterEnemyMemory::find_best_enemy()
{
	ENEMIES_MAP_IT	it = m_objects.end();
	float			min_dist = flt_max;

	for (ENEMIES_MAP_IT I = m_objects.begin(); I != m_objects.end(); I++) {
		if (I->second.danger < min_dist) {
			min_dist = I->second.danger;
			it = I;
		}
	}

	return it;
}


