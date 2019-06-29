#pragma once

#include "ai_monster_defs.h"

class CAI_Biting;

class CMonsterEnemyMemory {
	CAI_Biting		*monster;
	TTime			time_memory;
	
	ENEMIES_MAP		m_objects;

public:
						CMonsterEnemyMemory		();
						~CMonsterEnemyMemory	();

	void				init_external			(CAI_Biting *M, TTime mem_time);
	void				update					();

	// -----------------------------------------------------
	const CEntityAlive	*get_enemy				();
	SMonsterEnemy		get_enemy_info			();
	u32					get_enemies_count		() {return m_objects.size();}

	const ENEMIES_MAP	&get_memory				() {return m_objects;}

private:

	void				add_enemy				(const CEntityAlive *enemy);
	void				remove_non_actual		();

	ENEMIES_MAP_IT		find_best_enemy			();

};

