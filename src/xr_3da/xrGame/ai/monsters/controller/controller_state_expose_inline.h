#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerExposeAbstract CStateControlExpose<_Object>

#define COVER_RESELECT_QUANT 5000

TEMPLATE_SPECIALIZATION
CStateControllerExposeAbstract::CStateControlExpose(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateControllerExposeAbstract::~CStateControlExpose()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::initialize()
{
	inherited::initialize();

	last_time_reselect_cover	= 0;
	target.node					= u32(-1);
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::execute()
{
	object->set_controlled_task(eTaskAttack);

	// Hide	
	bool b_reselect_target = false;
	if (last_time_reselect_cover + COVER_RESELECT_QUANT < object->m_current_update) b_reselect_target = true;
	if (object->IsPathEnd(2.5f)) b_reselect_target = true;

	if (b_reselect_target) {
		if (!object->GetCoverFromEnemy(object->EnemyMan.get_enemy()->Position(), target.position, target.node)) {
			target.node					= u32(-1);
			last_time_reselect_cover	= object->m_current_update;
		}
	}
	
	if (target.node != u32(-1)) {
		object->MoveToTarget			(target.position, target.node);
	} else {
		object->MoveAwayFromTarget	(object->EnemyMan.get_enemy()->Position());
	}

	object->MotionMan.accel_activate		(eAT_Aggressive);
	object->MotionMan.accel_set_braking		(false);
	
	object->MotionMan.m_tAction				= ACT_RUN;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerExposeAbstract::check_start_conditions()
{
	if (object->HasUnderControl()) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerExposeAbstract::check_completion()
{
	if (!object->HasUnderControl()) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::finalize()
{
	inherited::finalize();
	object->set_controlled_task(u32(eTaskNone));	
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->set_controlled_task(u32(eTaskNone));
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerExposeAbstract
