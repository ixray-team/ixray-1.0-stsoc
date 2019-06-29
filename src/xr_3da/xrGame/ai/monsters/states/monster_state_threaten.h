#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class	CStateMonsterThreaten : public CStateManagerState<_Object> {
protected:
	typedef CStateManagerState<_Object> inherited;

	enum EAttackStates {
		eStateWalk			= u32(0),
		eStateThreaten,

		eStateNumber
	};

	typedef typename CStateBase<_Object>* SSubStatePtr;
	SSubStatePtr		states[eStateNumber];

public:
						CStateMonsterThreaten	(LPCSTR state_name, SSubStatePtr state_walk, SSubStatePtr state_threaten);
	virtual				~CStateMonsterThreaten	();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_threaten_inline.h"
