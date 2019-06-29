#pragma once
#include "state_manager.h"
#include "state.h"

template <typename _Object>
class CStateManagerSecond : public IStateManagerBase, public CState<_Object> {
	typedef CState<_Object> inherited;

public:
					CStateManagerSecond		(_Object *obj) : inherited(obj) {}

	virtual void	reinit					();
	virtual void	update					();
	virtual void	force_script_state		(EMonsterState state);
	virtual void	execute_script_state	();
	virtual	void	critical_finalize		();
	
	virtual	EMonsterState get_state_type	();

protected:
			bool	can_eat					();
			bool	check_state				(u32 state_id);
};

#include "state_manager_second_inline.h"

