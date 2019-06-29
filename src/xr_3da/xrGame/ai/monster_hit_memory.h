#pragma once
#include "ai_monster_defs.h"

class CAI_Biting;

class CMonsterHitMemory {
	CAI_Biting				*monster;
	TTime					time_memory;

	MONSTER_HIT_VECTOR		m_hits;

public:
						CMonsterHitMemory		();
						~CMonsterHitMemory		();

	void				init_external			(CAI_Biting *M, TTime mem_time);
	void				update					();

	// -----------------------------------------------------
	bool				is_hit					() {return !m_hits.empty();}
	bool				is_hit					(CObject *pO);

	void				add_hit					(CObject *who, EHitSide side);
	
	Fvector				get_last_hit_dir		();
	TTime				get_last_hit_time		();
	CObject				*get_last_hit_object	();
	Fvector				get_last_hit_position	();
	
	void				clear					() {m_hits.clear();}

private:
	void				remove_non_actual		();
};
