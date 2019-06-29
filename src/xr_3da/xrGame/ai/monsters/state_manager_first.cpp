#include "stdafx.h"
#include "state_manager_first.h"
#include "BaseMonster/base_monster.h"
#include "ai_monster_state.h"


CStateManagerFirst::CStateManagerFirst(CBaseMonster	*monster) 
{
	m_object		= monster;
	m_current_state	= eStateUnknown;
}

CStateManagerFirst::~CStateManagerFirst() 
{
	remove_all();
}

void CStateManagerFirst::reinit()
{
	m_current_state	= eStateRest;
}

void CStateManagerFirst::force_script_state(EMonsterState state)
{
	set_state(state);
}


void CStateManagerFirst::execute_script_state()
{
	execute();
}

void CStateManagerFirst::execute()
{
	STATE_MAP_IT state_it = m_states.find(m_current_state);
	VERIFY(state_it != m_states.end());

	state_it->second->Execute	(Device.dwTimeGlobal);
}

void CStateManagerFirst::add_state(EMonsterState id, IState *p_state)
{
	m_states.insert(mk_pair(id, p_state));
}

void CStateManagerFirst::remove_all() 
{
	for (STATE_MAP_IT it = m_states.begin(); it != m_states.end(); it++) {
		xr_delete(it->second);	
	}

	m_states.clear();
}

IState *CStateManagerFirst::get_state(EMonsterState state_id)
{
	STATE_MAP_IT state_it = m_states.find(state_id);
	VERIFY(state_it != m_states.end());

	return state_it->second;
}

void CStateManagerFirst::set_state(EMonsterState state)
{
	if (m_current_state != state) {
		STATE_MAP_IT state_it_prev = m_states.find(m_current_state);
		VERIFY(state_it_prev != m_states.end());

		STATE_MAP_IT state_it_new = m_states.find(state);
		VERIFY(state_it_new != m_states.end());

		state_it_prev->second->Done		();
		state_it_prev->second->Reset	();
		state_it_new->second->Activate	();		
		m_current_state					= state;
	}
}


