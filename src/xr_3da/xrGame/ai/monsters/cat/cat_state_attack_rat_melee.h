#pragma once

#include "../state.h"

template<typename _Object>
class CStateCatAttackRatMelee : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateCatAttackRatMelee	(_Object *obj);
	virtual	void		execute						();

	virtual bool 		check_completion			();
	virtual bool 		check_start_conditions		();
};

#include "cat_state_attack_rat_melee_inline.h"
