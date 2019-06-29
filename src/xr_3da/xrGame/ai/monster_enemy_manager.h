#pragma once
#include "ai_monster_defs.h"

class CAI_Biting;

class CMonsterEnemyManager {
	CAI_Biting			*monster;

	const CEntityAlive *enemy;
	Fvector				position;
	u32					vertex;
	TTime				time_last_seen;

	Flags32				flags;
	bool				forced;

	bool				expediency;

	const CEntityAlive *prev_enemy;
	Fvector				prev_enemy_position;

	bool				enemy_see_me;

	EDangerType			danger_type;

public:
						CMonsterEnemyManager	(); 
						~CMonsterEnemyManager	();
	void				init_external			(CAI_Biting *M);
	void				reinit					();

	void				update					();

	void				force_enemy				(const CEntityAlive *enemy);
	void				unforce_enemy			();

	const CEntityAlive *get_enemy				() {return enemy;}
	EDangerType			get_danger_type			() {return danger_type;}
	const Fvector		&get_enemy_position		() {return position;}
	u32					get_enemy_vertex		() {return vertex;}
	TTime				get_enemy_time_last_seen() {return time_last_seen;}

	Flags32				&get_flags				() {return flags;}
	
	bool				see_enemy_now			();

	// вернуть количество врагов
	u32					get_enemies_count		();

	void				add_enemy				(const CEntityAlive *);
};
