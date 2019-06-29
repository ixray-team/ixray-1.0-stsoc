#pragma once
#include "ai_monster_defs.h"

class CAI_Biting;

class CMotionStats {
	CAI_Biting	*pMonster;

	struct elem {
		float	speed;
		Fvector position;
		TTime	time;
	};

	enum {MAX_ELEMS = 10};

	elem			_data[MAX_ELEMS];	
	u32				index;			// индекс всегда указывает на подготовленное место в массиве

public:
	CMotionStats(CAI_Biting	*pM) : pMonster(pM),index(0) {};

	void update			();
	bool is_good_motion	(u32 elems_checked);
};
