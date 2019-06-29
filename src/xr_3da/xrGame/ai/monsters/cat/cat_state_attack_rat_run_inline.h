#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateCatAttackRatRunAbstract CStateCatAttackRatRun<_Object>

TEMPLATE_SPECIALIZATION
CStateCatAttackRatRunAbstract::CStateCatAttackRatRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatRunAbstract::initialize()
{
	inherited::initialize();
	object->path().prepare_builder	();	
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());
	
	// установка параметров функциональных блоков
	object->set_action						(ACT_RUN);
	object->anim().accel_deactivate			();
	object->path().set_target_point			(object->EnemyMan.get_enemy()->Position(), object->EnemyMan.get_enemy()->ai_location().level_vertex_id());
	object->path().set_rebuild_time			(100 + u32(50.f * dist));
	object->path().set_distance_to_end		(1.5f);
	object->path().set_use_covers			();
	object->path().set_cover_params			(5.f, 30.f, 1.f, 30.f);
	object->path().set_try_min_time			(false);
	object->set_state_sound					(MonsterSound::eMonsterSoundAggressive);
}
