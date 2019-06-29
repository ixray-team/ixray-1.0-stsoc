#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterThreatenWalk : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterThreatenWalk	(LPCSTR state_name);
	virtual				~CStateMonsterThreatenWalk	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_threaten_walk_inline.h"
