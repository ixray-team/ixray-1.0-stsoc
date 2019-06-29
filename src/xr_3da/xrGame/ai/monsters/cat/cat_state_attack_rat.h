#pragma once

#include "../state.h"

template<typename _Object>
class	CStateCatAttackRat : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;

	enum EAttackStates {
		eStateRun			= u32(0),
		eStateMelee,
	};

public:
						CStateCatAttackRat		(_Object *obj);
	virtual	void		execute					();
};

#include "cat_state_attack_rat_inline.h"
