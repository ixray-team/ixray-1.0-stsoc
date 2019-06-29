#include "stdafx.h"
#include "ai_monster_motion.h"
#include "biting/ai_biting.h"
#include "../PHMovementControl.h"
#include "ai_monster_utils.h"
#include "ai_monster_jump.h"

//////////////////////////////////////////////////////////////////////////
// m_tAction processing
//////////////////////////////////////////////////////////////////////////

void CMotionManager::Update()
{
	if (seq_playing) return;
	if (TA_IsActive()) return;

	// Установка Yaw
	if (pMonster->IsMovingOnPath()) pMonster->SetDirectionLook( ((spec_params & ASP_MOVE_BKWD) == ASP_MOVE_BKWD) );

	SelectAnimation		();
	SelectVelocities	();
}


//////////////////////////////////////////////////////////////////////////
// SelectAnimation
// In:	path, target_yaw, m_tAction
// Out:	установить анимацию в cur_anim_info().motion
void CMotionManager::SelectAnimation()
{
	EAction							action = m_tAction;
	if (pMonster->IsMovingOnPath()) action = GetActionFromPath();

	cur_anim_info().motion			= get_sd()->m_tMotions[action].anim;
	
	pMonster->CheckSpecParams		(spec_params);	
	if (Seq_Active()) return;

	if (prev_motion	!= cur_anim_info().motion) CheckTransition(prev_motion, cur_anim_info().motion);
	if (Seq_Active()) return;

	CheckReplacedAnim				();

	pMonster->ProcessTurn			();
}


//////////////////////////////////////////////////////////////////////////
// SelectVelocities
// In:	path, target_yaw, анимация
// Out:	установить linear и angular velocities, 
//		по скорости движения выбрать финальную анимацию из Velocity_Chain
//		установить скорость анимации в соответствие с физ скоростью
void CMotionManager::SelectVelocities()
{
	// получить скорости движения по пути
	bool		b_moving = pMonster->IsMovingOnPath();
	SMotionVel	path_vel;	path_vel.set(0.f,0.f);
	SMotionVel	anim_vel;	anim_vel.set(0.f,0.f);

	if (b_moving) {

		u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
			next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

		// если сейчас стоит на месте и есть след точка (т.е. должен быть в движении),
		// то реализовать поворот на месте, а дальше форсировать скорость со следующей точки
		if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
				cur_point_velocity_index = next_point_velocity_index;
		} 

		const CDetailPathManager::STravelParams &current_velocity = pMonster->CDetailPathManager::velocity(cur_point_velocity_index);
		path_vel.set(_abs(current_velocity.linear_velocity), current_velocity.real_angular_velocity);
	}

	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	// получить скорости движения по анимации
	anim_vel.set(item_it->second.velocity->velocity.linear, item_it->second.velocity->velocity.angular_real);

	//	// проверить на совпадение
	//	R_ASSERT(fsimilar(path_vel.linear,	anim_vel.linear));
	//	R_ASSERT(fsimilar(path_vel.angular,	anim_vel.angular));

	// установка линейной скорости	
	// - проверить на возможность торможения
	if (!accel_check_braking(0.f)) {

		pMonster->m_velocity_linear.target	= _abs(anim_vel.linear);
		if (fis_zero(pMonster->m_velocity_linear.target)) pMonster->stop_linear();
	} else {
		pMonster->stop_linear_accel();
	}

	// если невидимый, то установить скорость из пути
	if (pMonster->state_invisible) pMonster->m_velocity_linear.target	= _abs(path_vel.linear);

	// финальная корректировка скорости анимации по физической скорости
	if (b_moving) {
		Fvector temp_vec;
		pMonster->m_PhysicMovementControl->GetCharacterVelocity(temp_vec);
		float  real_speed = temp_vec.magnitude();

		EMotionAnim new_anim;
		float		a_speed;

		if (accel_chain_get(real_speed, cur_anim_info().motion, new_anim, a_speed)) {
			cur_anim_info().motion			= new_anim;
			cur_anim_info().speed.target	= a_speed;
		} else 
			cur_anim_info().speed.target	= -1.f;
	} else 
		cur_anim_info().speed.target	= -1.f;


	// установка угловой скорости
	if (!b_forced_velocity) {
		item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
		VERIFY(get_sd()->m_tAnims.end() != item_it);

		pMonster->m_velocity_angular = item_it->second.velocity->velocity.angular_real;
	}

	// применить 
	// если установленная анимация отличается от предыдущей - установить новую анимацию
	if (prev_motion	!= cur_anim_info().motion) {
		prev_motion	= cur_anim_info().motion;
		ForceAnimSelect();		
	}
}

#define ANIM_CHANGE_SPEED_VELOCITY 10.f

void CMotionManager::FrameUpdate()
{
	// update animation speed 
	if (m_cur_anim.speed.target > 0) {
		if (m_cur_anim.speed.current < 0) m_cur_anim.speed.current = 0.f;
		velocity_lerp(m_cur_anim.speed.current, m_cur_anim.speed.target, ANIM_CHANGE_SPEED_VELOCITY, Device.fTimeDelta);
	} else m_cur_anim.speed.current = -1.f;

	if (!seq_playing && !TA_IsActive() && (!pJumping || (pJumping && !pJumping->IsActive()))) {
		Update				();	
		// ValidateAnimation();
	} else {
		pMonster->disable_path();
	}
}

void CMotionManager::ScheduledInit()
{
	spec_params			= 0;
	accel_deactivate	();
}

void CMotionManager::UpdateScheduled()
{
	if (!seq_playing && !TA_IsActive() && (!pJumping || (pJumping && !pJumping->IsActive()))) {
		b_forced_velocity	= false;
		Update				();	
	} else {
		pMonster->disable_path();
	}
}
