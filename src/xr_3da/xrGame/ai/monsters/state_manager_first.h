#pragma once
#include "state_manager.h"
#include "state_defs.h"

class CBaseMonster;
class IState;

class CStateManagerFirst : public IStateManagerBase {
	
	DEFINE_MAP		(EMonsterState, IState*, STATES_MAP, STATE_MAP_IT);
	STATES_MAP		m_states;

protected:

	CBaseMonster	*m_object;
	EMonsterState	m_current_state;

public:

					CStateManagerFirst			(CBaseMonster	*monster);
	virtual			~CStateManagerFirst			();

	virtual void	force_script_state			(EMonsterState state);
	virtual void	execute_script_state		();
	virtual void	reinit						();	
	virtual	void	critical_finalize			() {}
	
	virtual	EMonsterState	get_state_type		() {return eStateUnknown;}


protected:

	virtual	void	add_state					(EMonsterState id, IState *p_state);
	virtual void	remove_all					();
	virtual void	set_state					(EMonsterState state);
	virtual void	execute						();

			IState	*get_state					(EMonsterState state_id);
};
