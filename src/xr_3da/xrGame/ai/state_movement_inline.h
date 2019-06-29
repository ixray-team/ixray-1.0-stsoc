#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMovementAbstract CStateMovement<_Object>


TEMPLATE_SPECIALIZATION
CStateMovementAbstract::CStateMovement(_Object *obj, EStateType s_type = ST_Unknown, void *data = 0)
	 : inherited(obj, s_type, data)
{
}

TEMPLATE_SPECIALIZATION
CStateMovementAbstract::~CStateMovement()
{
}

TEMPLATE_SPECIALIZATION
void CStateMovementAbstract::initialize()
{
	inherited::initialize	();

	m_path_state			= ePathStateUndefined;
	m_target.node			= u32(-1);
	m_target.position.set	(0.f, 0.f, 0.f);

	m_path_targeted			= false;
}


TEMPLATE_SPECIALIZATION
void CStateMovementAbstract::path_update()
{
	update_path_info	();

	path_failed			();
	path_end			();

	select_target_point	();
}

TEMPLATE_SPECIALIZATION
void CStateMovementAbstract::update_path_info()
{
	bool b_path_ready	= object->CMonsterMovement::actual() && object->CMonsterMovement::enabled() && !object->CMonsterMovement::completed(); 
	bool b_new_path		= object->CDetailPathManager::time_path_built() > time_state_started;
	
	if (b_new_path && b_path_ready) m_path_state = ePathStateBuilt;
}



