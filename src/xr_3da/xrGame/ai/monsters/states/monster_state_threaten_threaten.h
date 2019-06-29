#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterThreatenThreaten : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterThreatenThreaten	(LPCSTR state_name);
	virtual				~CStateMonsterThreatenThreaten	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_threaten_threaten_inline.h"
