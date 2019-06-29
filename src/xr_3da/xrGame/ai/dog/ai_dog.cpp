#include "stdafx.h"
#include "ai_dog.h"
#include "../ai_monster_utils.h"

CAI_Dog::CAI_Dog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingRunAway>	(this);
	stateControlled		= xr_new<CBitingControlled>	(this);
	
	CurrentState		= stateRest;
	CurrentState->Reset	();
	
	CJumping::Init		(this);
	controlled::init_external(this);
}

CAI_Dog::~CAI_Dog()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateExploreDNE);
	xr_delete(stateControlled);
}

void CAI_Dog::reinit()
{
	inherited::reinit();

	Bones.Reset();
	strike_in_jump					= false;
}


void CAI_Dog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);

	// todo: PUT visual from load OFF
	CJumping::AddState(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE,	false,	0.f, inherited::get_sd()->m_fsVelocityRunFwdNormal.velocity.angular_real);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		// define animation set
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND);

		MotionMan.AddAnim(eAnimRunTurnLeft,		"stand_run_turn_left_",	-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);

		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND);
		MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

		MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);	
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);

		MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

		// define transitions
		// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
		MotionMan.AddTransition(eAnimLieIdle,	eAnimSleep,	eAnimLieToSleep,	false);
		MotionMan.AddTransition(eAnimSleep,		PS_STAND,	eAnimSleepStandUp,	false);
		MotionMan.AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
		MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);
		MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false);

		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft,	eAnimStandTurnRight, PI_DIV_6);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun); //,		eAnimRunTurnLeft, eAnimRunTurnRight, 5 * PI / 180);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft,	eAnimStandTurnRight, PI_DIV_6);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);	
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimSniff);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();

	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}



void CAI_Dog::StateSelector()
{	
	if (is_under_control()) {
		SetState(stateControlled);
		return;
	}
	
	IState *state = 0;
	
	TTime last_hit_time = 0;
	if (HitMemory.is_hit()) last_hit_time = HitMemory.get_last_hit_time();

	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = statePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = stateAttack; break;
		}
	} else if (HitMemory.is_hit() && (last_hit_time + 10000 > m_current_update)) state = stateExploreDNE;
	else if (hear_dangerous_sound || hear_interesting_sound) {
		if (hear_dangerous_sound)			state = stateExploreNDE;		
		if (hear_interesting_sound)			state = stateExploreNDE;	
	} else if (CorpseMan.get_corpse() && ((GetSatiety() < get_sd()->m_fMinSatiety) || flagEatNow))					
											state = stateEat;	
	else									state = stateRest;


	// Temp
	ChangeEntityMorale(-0.5f);

	if (state == stateAttack) {
		float yaw,pitch;
		Fvector().sub(EnemyMan.get_enemy_position(), Position()).getHP(yaw,pitch);
		
		yaw = angle_normalize(-yaw);

		if (angle_difference(yaw, m_body.current.yaw) > PI_DIV_2) {
			
			if (from_right(yaw, m_body.current.yaw)) MotionMan.SetSpecParams(ASP_ROTATION_RUN_RIGHT);
			else MotionMan.SetSpecParams(ASP_ROTATION_RUN_LEFT);
		}
	}

	SetState(state);
}


void CAI_Dog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
	
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
	}

	if ((spec_params & ASP_ROTATION_JUMP) == ASP_ROTATION_JUMP) {
		float yaw, pitch;
		Fvector().sub(EnemyMan.get_enemy()->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;

		if (angle_difference(angle_normalize(yaw), m_body.current.yaw) > 120 * PI / 180) {
			
			EMotionAnim anim;
			if (from_right(yaw,m_body.current.yaw)) {
				anim = eAnimJumpRight;
				yaw = angle_normalize(yaw + 15 * PI / 180);	
			} else {
				anim = eAnimJumpLeft;
				yaw = angle_normalize(yaw - 15 * PI / 180);
			}
			
			MotionMan.Seq_Add(anim);
			MotionMan.Seq_Switch();

			CMovementManager::m_body.target.yaw = yaw;

			// calculate angular speed
			float new_angular_velocity; 
			float delta_yaw = angle_difference(yaw,m_body.current.yaw);
			float time = MotionMan.GetCurAnimTime();
			new_angular_velocity = 1.5f * delta_yaw / time; 

			MotionMan.ForceAngularSpeed(new_angular_velocity);
		}
	}

	if (MotionMan.GetCurAnim() == eAnimRun) {
		if ((spec_params & ASP_ROTATION_RUN_LEFT) == ASP_ROTATION_RUN_LEFT) 
			MotionMan.SetCurAnim(eAnimRunTurnLeft);
		else if  ((spec_params & ASP_ROTATION_RUN_RIGHT) == ASP_ROTATION_RUN_RIGHT) 
			MotionMan.SetCurAnim(eAnimRunTurnRight);
	}
		 

}

void CAI_Dog::OnSoundPlay()
{
	//if (!Bones.IsActive()) Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Y, PI_DIV_6, PI_MUL_2, 1);
}

void CAI_Dog::UpdateCL()
{
	inherited::UpdateCL();

	CJumping::Update();
	float trace_dist = 1.0f;

	// Проверить на нанесение хита во время прыжка
	if (CJumping::IsGlide()) {

		if (strike_in_jump) return;
		
		const CEntity *pE = smart_cast<const CEntity *>(CJumping::GetEnemy());
		if (!pE) return;

		Fvector trace_from;
		Center(trace_from);
		setEnabled(false);
		Collide::rq_result	l_rq;

		if (Level().ObjectSpace.RayPick(trace_from, Direction(), trace_dist , Collide::rqtBoth, l_rq)) {
			if ((l_rq.O == CJumping::GetEnemy()) && (l_rq.range < trace_dist)) {
				//HitEntity(pE, inherited::get_sd()->m_fHitPower,Direction());
				strike_in_jump = true;
			}
		}
		setEnabled(true);			

		// !!!
		// LookPosition(pE->Position());
	}
}

void CAI_Dog::OnJumpStop()
{
	MotionMan.DeactivateJump();
	MotionMan.Update();
	strike_in_jump = false;
}


//#define TURN_HEAD_ANGLE PI_DIV_4
//
//void CAI_Dog::BonesInMotion() 
//{
//	EMotionAnim anim = MotionMan.GetCurAnim();
//	bool b_enable_motions = false;
//
//	switch (anim) {
//	case eAnimSitIdle:
//	case eAnimWalkFwd:
//		b_enable_motions = true;
//		break;
//	}
//	
//
//	if (!Bones.IsActive() && b_enable_motions) {
//		float	x,y,z;
//		u8		side_to_look = 0; // 0 - front, 1 - left, 2 - right
//		
//		u8 selector = u8(Random.randI(100)); 
//		if (selector < 20)		side_to_look = 1;
//		else if (selector < 40)	side_to_look = 2;
//		
//		if (side_to_look == 1)	{	x = -TURN_HEAD_ANGLE;	y = TURN_HEAD_ANGLE;	z = TURN_HEAD_ANGLE;} 
//		else if (side_to_look == 2)	{	x = TURN_HEAD_ANGLE; 	y = TURN_HEAD_ANGLE;	z = -TURN_HEAD_ANGLE;}
//		else {x = 0.f; y = 0.f; z = 0.f;}
//
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_X, x, PI_DIV_2, 3000);
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Y, y, PI_DIV_2, 3000);
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Z, z, PI_DIV_2, 3000);
//	}
//}

