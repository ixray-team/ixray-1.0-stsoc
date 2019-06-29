#pragma once

#include "../state.h"

template<typename _Object>
class CStateCatAttackRatRun : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateCatAttackRatRun	(_Object *obj);

	virtual void		initialize				();
	virtual	void		execute					();
};

#include "cat_state_attack_rat_run_inline.h"
