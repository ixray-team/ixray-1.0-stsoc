#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer_attack_misc.h"
#include "burer.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_movement.h"
#include "../../../sound_player.h"

#define DIST_QUANT 10.f

CBurerAttackRunAround::CBurerAttackRunAround(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackRunAround::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	

	time_started = Device.dwTimeGlobal;

	dest_direction.set(0.f,0.f,0.f);

	// select point
	
	Fvector dir_to_enemy;
	dir_to_enemy.sub(enemy->Position(),pMonster->Position());
	dir_to_enemy.normalize();
	
	Fvector dir_from_enemy;
	dir_from_enemy.sub(pMonster->Position(),enemy->Position());
	dir_from_enemy.normalize();

	float dist = pMonster->Position().distance_to(enemy->Position());
	

	if (dist > 30.f) {								// бежать к врагу
		selected_point.mad(pMonster->Position(),dir_to_enemy,DIST_QUANT);
	} else if ((dist < 20.f) && (dist > 4.f)) {	// убегать от врага
		selected_point.mad(pMonster->Position(),dir_from_enemy,DIST_QUANT);
		dest_direction.set(dir_to_enemy);
	} else {											// выбрать случайную позицию
		selected_point = random_position(pMonster->Position(), DIST_QUANT);
		dest_direction.set(dir_to_enemy);
	}
	pMonster->movement().disable_path();
	
	pMonster->movement().initialize_movement();
}


void CBurerAttackRunAround::Run()
{
	if (!fis_zero(dest_direction.square_magnitude())) {
		pMonster->movement().set_use_dest_orient		(true);
		pMonster->movement().set_dest_direction	(dest_direction);
	}

	pMonster->MotionMan.m_tAction = ACT_RUN;
	pMonster->movement().set_target_point		(selected_point);
	pMonster->movement().set_generic_parameters	();
	pMonster->movement().set_use_covers			(false);

	pMonster->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
}

void CBurerAttackRunAround::Done()
{
}

void CBurerAttackRunAround::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}

bool CBurerAttackRunAround::IsCompleted() 
{
	if ((time_started + 3500 < Device.dwTimeGlobal) || 
		(pMonster->movement().IsMovingOnPath() && pMonster->movement().IsPathEnd(2.f))) {

		pMonster->DirMan.face_target(enemy);
		return true;
	}

	else return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CBurerAttackFaceTarget::CBurerAttackFaceTarget(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackFaceTarget::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	
	point = enemy->Position();
}


void CBurerAttackFaceTarget::Run()
{
	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	pMonster->DirMan.face_target(point);
	pMonster->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
}

void CBurerAttackFaceTarget::Done()
{
}

void CBurerAttackFaceTarget::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}

bool CBurerAttackFaceTarget::IsCompleted() 
{
	if (angle_difference(pMonster->movement().m_body.current.yaw,pMonster->movement().m_body.target.yaw) < deg(5)) {
		return true;
	}
	return false;
}

