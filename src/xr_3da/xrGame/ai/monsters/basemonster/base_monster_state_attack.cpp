#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"

#include "../../rat/ai_rat.h"
#include "../../../actor.h"
#include "../ai_monster_jump.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"
#include "../ai_monster_utils.h"

#include "../../../WeaponMagazined.h"
#include "../../../inventory.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"
#include "../../../entitycondition.h"
#include "../../../sound_player.h"
#include "../../../ai_object_location.h"


/////////////////////////////////////////////////////////////////////////
// State Attack Flags 

#define AF_ENEMY_DOESNT_SEE_ME			(1 << 0)
#define AF_ENEMY_GO_FARTHER_FAST		(1 << 1)
#define AF_REMEMBER_HIT_FROM_THIS_ENEMY	(1 << 2)
#define AF_THIS_IS_THE_ONLY_ENEMY		(1 << 3)
#define AF_ENEMY_IS_NOT_REACHABLE		(1 << 4)
#define AF_CAN_ATTACK_FROM_BACK			(1 << 5)
#define AF_SEE_ENEMY					(1 << 6)

#define AF_GOOD_MOVEMENT_ON_PATH		(1 << 7)
#define AF_NEED_REBUILD_PATH			(1 << 8)

#define AF_ATTACK_RAT					(1 << 9)
#define AF_NEW_ENEMY					(1 << 10)

#define AF_LOW_MORALE					(1 << 11)

#define AF_HAS_JUMP_ABILITY				(1 << 12)
#define AF_HAS_INVISIBILITY_ABILITY		(1 << 13)

#define AF_CAN_EXEC_ROTATION_JUMP		(1 << 14)
#define AF_BAD_MOTION					(1 << 15)

#define AF_CAN_ATTACK_RUN				(1 << 16)

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseMonsterAttack::CBaseMonsterAttack(CBaseMonster *p)  
{
	pMonster = p;

	SetPriority(PRIORITY_HIGH);
}


void CBaseMonsterAttack::Init()
{
	LOG_EX("attack init");
	IState::Init();

	// Получить врага
	enemy		= pMonster->EnemyMan.get_enemy();
	
	UpdateInitFlags();

	// Установка дистанции аттаки
	if (init_flags.is(AF_ATTACK_RAT)) {
		m_fDistMin = 0.7f;				// todo: make as ltx parameters
		m_fDistMax = 2.8f;				// todo: make as ltx parameters
	}
	
	pMonster->MeleeChecker.init_attack(); // учитывать атак-стопы

	b_in_threaten				= false;
	ThreatenTimeStarted			= 0;
	ThreatenMinDelay			= 0;
	LastTimeRebuild				= 0;
	bEnableBackAttack			= true;

	m_tAction					= ACTION_RUN;
	m_tPrevAction				= ACTION_RUN;

	next_rot_jump_enabled		= 0;
	time_start_walk_away		= 0;

	time_next_attack_run		= 0;
	time_next_psi_attack		= 0;

	pMonster->movement().initialize_movement();
	
}

#define TIME_WALK_PATH						5000
#define PREDICT_POSITION_MIN_THRESHOLD		6.0f		// мин дистанция при которой 	
#define BUILD_FULL_PATH_MAX_DIST			10.0f		// макс дистанция до врага, при которой будет строится полный путь
#define BUILD_HALF_PATH_DIST				5.f			// дистанция не полного пути

#define THREATEN_DISTANCE					3.0f
#define MAX_DIST_ROTATION_JUMP				5.f

void CBaseMonsterAttack::Run()
{
	
	// Если враг изменился, инициализировать состояние
	if (pMonster->EnemyMan.get_enemy() != enemy) Init();

	
	UpdateFrameFlags();
	flags.or(frame_flags, init_flags.get());

	// обновить минимальную и максимальную дистанции до врага
	m_fDistMin = pMonster->MeleeChecker.get_min_distance	();
	m_fDistMax = pMonster->MeleeChecker.get_max_distance	();
	dist = pMonster->MeleeChecker.distance_to_enemy			(pMonster->EnemyMan.get_enemy());

	// определить переменные машины состояний
	bool b_attack_melee		= false;
	
	if ((m_tPrevAction == ACTION_ATTACK_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_ATTACK_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	if (m_tAction == ACTION_ATTACK_MELEE) b_attack_melee = true; 
	
	
	// проверить на возможность пугания и подкрадывания
	if (CheckThreaten()) m_tAction	= ACTION_THREATEN;
	else if (CheckSteal()) m_tAction = ACTION_STEAL;
	
	
	// если враг не виден на протяжении 1 сек - бежать к нему
	if (!flags.is(AF_SEE_ENEMY) && (pMonster->EnemyMan.get_enemy_time_last_seen() + 1000 < m_dwCurrentTime))	m_tAction = ACTION_RUN;
	
	// Проверить, достижим ли противник
	if (flags.is(AF_ENEMY_IS_NOT_REACHABLE) && !b_attack_melee) m_tAction = ACTION_ENEMY_POSITION_APPROACH;
	//if ((flags.is(AF_BAD_MOTION)) && !b_attack_melee) time_start_walk_away = m_dwCurrentTime;
	//if (time_start_walk_away + 5000 > m_dwCurrentTime) {
	//	m_tAction = ACTION_ENEMY_WALK_AWAY;
	//}


	// проверить на возможность прыжка
	if (flags.is(AF_HAS_JUMP_ABILITY)) 
		if (pJumping->Check(pMonster->Position(),enemy->Position(),enemy)){
			pMonster->sound().play(MonsterSpace::eMonsterSoundAttackHit);
			pMonster->MotionMan.ActivateJump();
		}
	
	// восстановить некоторые переменные
	if (!b_attack_melee) bEnableBackAttack = true;
	
	if ((m_tAction == ACTION_RUN) && flags.is(AF_CAN_EXEC_ROTATION_JUMP) && (dist < MAX_DIST_ROTATION_JUMP))
		m_tAction = ACTION_ROTATION_JUMP;
	
	if ((m_tAction == ACTION_RUN) && CheckPsiAttack()) m_tAction = ACTION_PSI_ATTACK; 

	bool squad_active = false;
	CMonsterSquad	*pSquad = 0;

	//if (flags.is(AF_CAN_ATTACK_RUN)) m_tAction = ACTION_ATTACK_RUN;

	// Выполнение состояния
	switch (m_tAction) {	
		
		// ************
		case ACTION_RUN:		 // бежать на врага
		// ************	
			LOG_EX("ATTACK: RUN");
			pMonster->set_action							(ACT_RUN);
			pMonster->MotionMan.accel_activate				(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking			(false);

			pMonster->movement().set_target_point	(pMonster->EnemyMan.get_enemy()->Position(), pMonster->EnemyMan.get_enemy()->ai_location().level_vertex_id());
			pMonster->movement().set_rebuild_time	(pMonster->get_attack_rebuild_time());
			pMonster->movement().set_distance_to_end	(2.5f);
			pMonster->movement().set_use_covers		();
			pMonster->movement().set_cover_params	(5.f, 30.f, 1.f, 30.f);
			pMonster->movement().set_try_min_time	(false);
			pMonster->set_state_sound						(MonsterSpace::eMonsterSoundAttack);

			pSquad = monster_squad().get_squad(pMonster);
			squad_active = pSquad && pSquad->SquadActive();
			
			// Получить команду
			SSquadCommand command;
			pSquad->GetCommand(pMonster, command);
			if (!squad_active || (command.type != SC_ATTACK)) squad_active = false;


			if (squad_active) {
				pMonster->movement().set_use_dest_orient	(true);
				pMonster->movement().set_dest_direction	(command.direction);
			}
			
			
			
			break;

		// *********************
		case ACTION_ATTACK_MELEE:		// атаковать вплотную
		// *********************

			LOG_EX("ATTACK: ATTACK_MELEE");
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;
			bCanThreaten					= false;

			// если враг крыса под монстром подпрыгнуть и убить
//			if (m_bAttackRat) {
//				if (dist < 0.6f) {
//					if (!m_dwSuperMeleeStarted)	m_dwSuperMeleeStarted = m_dwCurrentTime;
//					if (m_dwSuperMeleeStarted + 600 < m_dwCurrentTime) {
//						// прыгнуть
//						pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT_JUMP);
//						m_dwSuperMeleeStarted = 0;
//					}
//				} else m_dwSuperMeleeStarted = 0;
//			}

			// Смотреть на врага 
			pMonster->DirMan.face_target(enemy, 1200);
			
			if (flags.is(AF_CAN_ATTACK_FROM_BACK)) {
				pMonster->MotionMan.SetSpecParams(ASP_BACK_ATTACK);
				bEnableBackAttack = false;
			}

			if (flags.is(AF_ATTACK_RAT)) pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RAT);

			pMonster->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;

		// ****************
		case ACTION_STEAL:
		// ****************

			LOG_EX("ATTACK: STEAL");
			pMonster->MotionMan.m_tAction = ACT_STEAL;
			pMonster->movement().set_target_point		(pMonster->EnemyMan.get_enemy_position(), pMonster->EnemyMan.get_enemy_vertex());
			pMonster->movement().set_generic_parameters	();

			pMonster->sound().play(MonsterSpace::eMonsterSoundSteal, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			break;
		
		// ******************
		case ACTION_THREATEN: 
		// ******************

			LOG_EX("ATTACK: THREATEN");
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			// Смотреть на врага 
			pMonster->DirMan.face_target(enemy, 1200);

			pMonster->MotionMan.SetSpecParams(ASP_THREATEN);

			pMonster->sound().play(MonsterSpace::eMonsterSoundThreaten);
			
			break;

		// **********************************
		case ACTION_ENEMY_POSITION_APPROACH:
		// **********************************
			LOG_EX("ATTACK: ENEMY_POSITION_APPROACH");
			pMonster->MotionMan.m_tAction		= ACT_RUN;
			pMonster->movement().set_target_point		(enemy->Position());
			pMonster->movement().set_generic_parameters	();
			pMonster->MotionMan.accel_activate					(eAT_Calm);
			
			pMonster->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;

		// **********************************
		case ACTION_ENEMY_WALK_AWAY:
		// **********************************
			LOG_EX("ATTACK: ENEMY_WALK_AWAY");

			pMonster->MotionMan.m_tAction						= ACT_WALK_FWD;
			pMonster->movement().set_retreat_from_point	(random_position(pMonster->EnemyMan.get_enemy_position(), 2.f));
			pMonster->movement().set_generic_parameters	();
			pMonster->sound().play						(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			pMonster->MotionMan.accel_activate					(eAT_Calm);
		
			break;

		// **********************
		case ACTION_ROTATION_JUMP: 
		// **********************

			pMonster->sound().play(MonsterSpace::eMonsterSoundLanding);
			pMonster->MotionMan.SetSpecParams	(ASP_ROTATION_JUMP);
			next_rot_jump_enabled				= m_dwCurrentTime + Random.randI(3000,4000);
			pMonster->movement().disable_path				();
			break;
		
		// ********************		
		case ACTION_ATTACK_RUN:
		// ********************
			LOG_EX("ATTACK: Attack Run");

			pMonster->sound().play				(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);			
			pMonster->MotionMan.m_tAction				= ACT_RUN;
			pMonster->movement().set_try_min_time(false);

			pMonster->MotionMan.accel_activate			(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking		(false);

			// продлить путь на некоторое расстояние дальше
			{
				Fvector target_point;
				Fvector dir;
				dir.sub(enemy->Position(), pMonster->Position());
				dir.normalize();
				target_point.mad(pMonster->Position(),dir,1.0f);
			
				pMonster->movement().set_target_point		(target_point);
				pMonster->movement().set_generic_parameters	();
			}
			
			pMonster->MotionMan.SetSpecParams(ASP_ATTACK_RUN);
			
			break;

		// ********************		
		case ACTION_PSI_ATTACK:
		// ********************
			LOG_EX("PSI_ATTACK");
			
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;
			
			// Смотреть на врага 
			pMonster->DirMan.face_target(enemy,1200);

			pMonster->MotionMan.SetSpecParams(ASP_PSI_ATTACK);
			pMonster->sound().play(MonsterSpace::eMonsterSoundPsyAttack);
			//pMonster->play_effect_sound();

			time_next_psi_attack			= m_dwCurrentTime + Random.randI(2000,4000);
			break;
	}

	init_flags.set(AF_NEW_ENEMY,FALSE);

	if ((m_tPrevAction == ACTION_ATTACK_RUN) && (m_tAction != ACTION_ATTACK_RUN))
		time_next_attack_run = m_dwCurrentTime + Random.randI(1000,2000);

	m_tPrevAction	= m_tAction; 
	
}


void CBaseMonsterAttack::Done()
{
	inherited::Done();
}

#define THREATEN_TIME  5300

// Реализация пугания:
// мораль - маленькая, расстояние - расстояние для аттаки
// на протяжении N мин не был атакован этим монстром
// враг стоит не бежит, видит
// если состояния ATTACK_MELEE ещё не было
bool CBaseMonsterAttack::CheckStartThreaten()
{
	// временно отключить пугание
	if (true) return false;
	
	if (b_in_threaten) return false;

	// проверка флагов
	if (!flags.is(AF_LOW_MORALE) || flags.is(AF_ENEMY_DOESNT_SEE_ME) || flags.is(AF_ENEMY_GO_FARTHER_FAST) || flags.is(AF_REMEMBER_HIT_FROM_THIS_ENEMY)) {
		return false;
	}

	// проверка дистанции
	if ((dist < m_fDistMax) || (dist > THREATEN_DISTANCE)) {
		return false;
	}

	// проверка угла
	float h,p;
	Fvector().sub(enemy->Position(),pMonster->Position()).getHP(h,p);
	if (angle_difference(-pMonster->movement().m_body.current.yaw,h) > PI / 15) {
		return false;
	}

	// проверка задержки после последнего пугания
	if (ThreatenTimeStarted + ThreatenMinDelay > m_dwCurrentTime) {
		return false;
	}

	return true;
}

bool CBaseMonsterAttack::CheckEndThreaten()
{
	if (!b_in_threaten) return false;
	if (ThreatenTimeStarted + THREATEN_TIME < m_dwCurrentTime) return true;

	float h,p;
	Fvector().sub(enemy->Position(),pMonster->Position()).getHP(h,p);
	if (angle_difference(-pMonster->movement().m_body.current.yaw,h) > PI_DIV_6) return true;
	
	// проверка флагов
	if (!flags.is(AF_LOW_MORALE) || flags.is(AF_ENEMY_DOESNT_SEE_ME) || flags.is(AF_ENEMY_GO_FARTHER_FAST) || flags.is(AF_REMEMBER_HIT_FROM_THIS_ENEMY)) {
		return true;
	}

	// проверка дистанции
	if ((dist < m_fDistMin) || (dist > THREATEN_DISTANCE)) {
		return true;
	}

	return false;
}

bool CBaseMonsterAttack::CheckThreaten()
{
	if (b_in_threaten) {
		if (CheckEndThreaten()) {
			b_in_threaten			= false;
			ThreatenTimeStarted		= 0;
			ThreatenMinDelay		= 0;
		}
	} else {
		if (CheckStartThreaten()) {
			b_in_threaten			= true;
			ThreatenTimeStarted		= m_dwCurrentTime;
			ThreatenMinDelay		= Random.randI(5000,10000);
		}
	}
	return b_in_threaten;
}



#define STEAL_MAX_ANGLE		PI_DIV_3

bool CBaseMonsterAttack::CheckSteal()
{
	if (!flags.is(AF_ATTACK_RAT) &&  flags.is(AF_THIS_IS_THE_ONLY_ENEMY) &&  flags.is(AF_ENEMY_DOESNT_SEE_ME) && !flags.is(AF_ENEMY_GO_FARTHER_FAST)) {
		
		// Вычислить отклонение по пути
		float path_angle = 0.f;
		if (pMonster->movement().IsMovingOnPath() && (pMonster->movement().detail().curr_travel_point_index() < pMonster->movement().detail().path().size()-3)) {
			const xr_vector<DetailPathManager::STravelPathPoint> &path = pMonster->movement().detail().path();

			float prev_yaw, prev_h;
			pMonster->movement().detail().direction().getHP(prev_yaw,prev_h);
			for (u32 i=pMonster->movement().detail().curr_travel_point_index()+1; i<path.size()-1;i++) {
				float h,p;
				Fvector().sub(path[i+1].position, path[i].position).getHP(h,p);

				path_angle += angle_difference(prev_yaw,h);
				prev_yaw = h;

				if (path_angle > STEAL_MAX_ANGLE) break;
			}
		}

		if ((path_angle < STEAL_MAX_ANGLE) && 	(dist > (m_fDistMax + 2.f) && (dist < 10.f))) return true;
	}
	
	return false;
}

Fvector CBaseMonsterAttack::RandomPos(Fvector pos, float R)
{
	Fvector local;

	local = pos;
	local.x += ::Random.randF(-R,R);
	local.z += ::Random.randF(-R,R);

	return local;
}


bool CBaseMonsterAttack::CanAttackFromBack()
{
	if (!bEnableBackAttack) return false;

	// проверить если враг находится сзади
	Fvector dir;
	dir.sub(enemy->Position(), pMonster->Position());

	float yaw1,p1,yaw2,p2;
	dir.getHP(yaw1,p1);
	pMonster->Direction().getHP(yaw2,p2);
	float dif = angle_difference(yaw1,yaw2);

	if ((PI - PI_DIV_6 < dif) && (dif < PI + PI_DIV_6)) return true;
	
	return false;
}



//////////////////////////////////////////////////////////////////////////
// New State
//////////////////////////////////////////////////////////////////////////

void CBaseMonsterAttack::UpdateInitFlags() 
{
	init_flags.zero();

	const CAI_Rat *tpRat = smart_cast<const CAI_Rat *>(enemy);
	if (tpRat) init_flags.or(AF_ATTACK_RAT);

	// определить способности
	pJumping = smart_cast<CJumping *>(pMonster);
	if (pJumping)			init_flags.or(AF_HAS_JUMP_ABILITY);
	if (pMonster->ability_invisibility())	init_flags.or(AF_HAS_INVISIBILITY_ABILITY);

	init_flags.or(AF_NEW_ENEMY);
}

void CBaseMonsterAttack::UpdateFrameFlags() 
{
	frame_flags.zero();

	if (pMonster->EnemyMan.get_flags().is(FLAG_ENEMY_DOESNT_SEE_ME))		
		frame_flags.or(AF_ENEMY_DOESNT_SEE_ME);
	
	if (pMonster->EnemyMan.get_flags().is(FLAG_ENEMY_GO_FARTHER_FAST)) 	
		frame_flags.or(AF_ENEMY_GO_FARTHER_FAST);
	
//	if (pMonster->IsDangerousEnemy(enemy))											
//		frame_flags.or(AF_REMEMBER_HIT_FROM_THIS_ENEMY);
	
	if (pMonster->EnemyMan.get_enemies_count()==1)		
		frame_flags.or(AF_THIS_IS_THE_ONLY_ENEMY);
	
	if (pMonster->movement().ObjectNotReachable(enemy)) 
		frame_flags.or(AF_ENEMY_IS_NOT_REACHABLE);

	// флаги движения по пути
	if (pMonster->movement().IsMovingOnPath())			frame_flags.or(AF_GOOD_MOVEMENT_ON_PATH);
	if (IS_NEED_REBUILD())					frame_flags.or(AF_NEED_REBUILD_PATH);

	if (pMonster->conditions().GetEntityMorale() < 0.8f) frame_flags.or(AF_LOW_MORALE);

	if (CanAttackFromBack())				frame_flags.or(AF_CAN_ATTACK_FROM_BACK);

	if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime)	
											frame_flags.or(AF_SEE_ENEMY);

	//if (pMonster->CanExecRotationJump() && CheckRotationJump()) 
	//										frame_flags.or(AF_CAN_EXEC_ROTATION_JUMP);

	if (!pMonster->movement().MotionStats->is_good_motion(3))	
											frame_flags.or(AF_BAD_MOTION);

	if (CanAttackRun())						frame_flags.or(AF_CAN_ATTACK_RUN);
			
}

#define MIN_ROTATION_JUMP_ANGLE 2*PI_DIV_3

bool CBaseMonsterAttack::CheckRotationJump()
{
	//if (MotionMan.Seq_Active()) return false;
	
	if (pJumping && pJumping->IsActive()) return false;
	
	// check angle
	float yaw, pitch;
	Fvector().sub(enemy->Position(), pMonster->Position()).getHP(yaw,pitch);

	if (angle_difference(-yaw,pMonster->movement().m_body.current.yaw) < MIN_ROTATION_JUMP_ANGLE) return false;

	// timing
	if (next_rot_jump_enabled > m_dwCurrentTime)  return false;

	return true;
}


bool CBaseMonsterAttack::CheckCompletion()
{
	return false;
}

bool CBaseMonsterAttack::CanAttackRun()
{
	// 1. монстр находится на небольшом расстоянии 
	if (dist > m_fDistMax) return false;
	// 2. видит на врага
	if (!flags.is(AF_SEE_ENEMY)) return false;
	// 3. давно не атаковал в беге :)
	if (time_next_attack_run > m_dwCurrentTime) return false;

	return true;
}

bool CBaseMonsterAttack::CheckPsiAttack()
{
	if (!pMonster->ability_psi_attack()) return false;
	if (!flags.is(AF_SEE_ENEMY)) return false;

	if (time_next_psi_attack > m_dwCurrentTime) return false;
	
	// проверить направленность на цель
	float h,p;
	Fvector().sub(pMonster->EnemyMan.get_enemy_position(), pMonster->Position()).getHP(h,p);
	float my_h,my_p;
	pMonster->Direction().getHP(my_h,my_p);
	
	if (angle_difference(h,my_h) > deg(10) ) return false;
	
	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(enemy));
	if (!pA) return false;
	
	if (pMonster != smart_cast<CBaseMonster *>(pA->ObjectWeLookingAt())) return false;

	CWeaponMagazined *pWeapon = smart_cast<CWeaponMagazined *>(pA->m_inventory->ActiveItem());
	if (!pWeapon) return false;

	return true;
}
