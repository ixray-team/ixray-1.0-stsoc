#include "stdafx.h"
#include "monster_corpse_memory.h"
#include "biting/ai_biting.h"

CMonsterCorpseMemory::CMonsterCorpseMemory()
{
	monster			= 0;
	time_memory		= 10000; 
}

CMonsterCorpseMemory::~CMonsterCorpseMemory()
{
}

void CMonsterCorpseMemory::init_external(CAI_Biting *M, TTime mem_time) 
{
	monster = M; 
	time_memory = mem_time;
}


void CMonsterCorpseMemory::update() 
{
	for (xr_vector<const CGameObject *>::const_iterator I = monster->CItemManager::items().begin(); I != monster->CItemManager::items().end(); ++I) {
		if (monster->CMemoryManager::visible_now(*I)) {
			const CEntityAlive *p_corpse = smart_cast<const CEntityAlive*>(*I);
			if (!p_corpse || p_corpse->g_Alive()) continue;
			add_corpse(p_corpse);
		}
	}

	// удалить устаревших врагов
	remove_non_actual();
}

void CMonsterCorpseMemory::add_corpse(const CEntityAlive *corpse)
{
	SMonsterCorpse corpse_info;
	corpse_info.position	= corpse->Position();
	corpse_info.vertex		= corpse->level_vertex_id();
	corpse_info.time		= Level().timeServer();

	CORPSE_MAP_IT it = m_objects.find(corpse);
	if (it != m_objects.end()) {
		// обновить данные о враге
		it->second = corpse_info;
	} else {
		// добавить врага в список объектов
		m_objects.insert(mk_pair(corpse, corpse_info));
	}
}

void CMonsterCorpseMemory::remove_non_actual() 
{
	TTime cur_time = Level().timeServer();

	// удалить 'старых' врагов и тех, расстояние до которых > 30м и др.
	for (CORPSE_MAP_IT it = m_objects.begin(), nit; it != m_objects.end(); it = nit)
	{
		nit = it; ++nit;
		// проверить условия удаления
		if (!it->first					|| 
			it->first->g_Alive()		|| 
			it->first->getDestroy()		||
			(it->second.time + time_memory < cur_time) || 
			(it->first->m_fFood < 1)	 
			) 
			m_objects.erase (it);
	}
}

const CEntityAlive *CMonsterCorpseMemory::get_corpse()
{
	CORPSE_MAP_IT	it = find_best_corpse();
	if (it != m_objects.end()) return it->first;
	return (0);
}

SMonsterCorpse CMonsterCorpseMemory::get_corpse_info()
{
	SMonsterCorpse ret_val;
	ret_val.time = 0;

	CORPSE_MAP_IT	it = find_best_corpse();
	if (it != m_objects.end()) ret_val = it->second;

	return ret_val;
}

CORPSE_MAP_IT CMonsterCorpseMemory::find_best_corpse()
{
	CORPSE_MAP_IT	it = m_objects.end();
	float			min_dist = flt_max;

	for (CORPSE_MAP_IT I = m_objects.begin(); I != m_objects.end(); I++) {
		if (I->second.position.distance_to(monster->Position()) < min_dist) {
			min_dist = I->second.position.distance_to(monster->Position());
			it = I;
		}
	}

	return it;
}
