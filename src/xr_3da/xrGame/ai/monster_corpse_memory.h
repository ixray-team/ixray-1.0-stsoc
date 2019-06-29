#pragma once
#include "ai_monster_defs.h"

class CAI_Biting;

class CMonsterCorpseMemory {
	CAI_Biting		*monster;
	TTime			time_memory;

	CORPSE_MAP		m_objects;

public:
						CMonsterCorpseMemory	();
						~CMonsterCorpseMemory	();

	void				init_external			(CAI_Biting *M, TTime mem_time);
	void				update					();

	// -----------------------------------------------------
	const CEntityAlive	*get_corpse				();
	SMonsterCorpse		get_corpse_info			();
	u32					get_corpse_count		() {return m_objects.size();}

	void				clear					() {m_objects.clear();}
	void				add_corpse				(const CEntityAlive *corpse);

private:
	void				remove_non_actual		();

	CORPSE_MAP_IT		find_best_corpse		();

};
