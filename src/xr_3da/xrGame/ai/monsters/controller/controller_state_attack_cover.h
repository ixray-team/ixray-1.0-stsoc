#pragma once

template<typename _Object>
class CStateControlTakeCover : public CState<_Object> {
	typedef	CState<_Object> inherited;

	enum {
		eStateTakeCover,
		eStateMoveOut
	} m_action;

public:

					CStateControlTakeCover	(_Object *p);
	virtual			~CStateControlTakeCover	();

	virtual void	initialize				();
	virtual void	reselect_state			();
	virtual void	finalize				();
	virtual void	critical_finalize		();
	
	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();

	virtual void	setup_substates			();
};

#include "controller_state_attack_cover_inline.h"