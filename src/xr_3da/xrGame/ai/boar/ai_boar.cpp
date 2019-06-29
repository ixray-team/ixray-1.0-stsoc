#include "stdafx.h"
#include "ai_boar.h"
#include "../ai_monster_utils.h"

CAI_Boar::CAI_Boar()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingRunAway>	(this);
	stateNull			= xr_new<CBitingNull>		();
	stateControlled		= xr_new<CBitingControlled>	(this);

	CurrentState		= stateRest;
	CurrentState->Reset	();

	controlled::init_external(this);
}

CAI_Boar::~CAI_Boar()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateExploreDNE);
	xr_delete(stateNull);
	xr_delete(stateControlled);
}



void CAI_Boar::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttackRun,		"stand_run_attack_",	-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		// define transitions																											
		MotionMan.AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);										
		MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
		MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);
		MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimLieStandUp,		false);


		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimDragCorpse);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun	, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/6);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

		MotionMan.AA_Load	(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

void CAI_Boar::StateSelector()
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
											state= stateEat;	
	else									state = stateRest;

	if (state == stateAttack) {
		look_at_enemy = true;
		// calc new target delta
		float yaw, pitch;
		Fvector().sub(EnemyMan.get_enemy()->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;
		yaw = angle_normalize(yaw);

		if (from_right(yaw,m_body.current.yaw)) {
			_target_delta = angle_difference(yaw,m_body.current.yaw);
		} else _target_delta = -angle_difference(yaw,m_body.current.yaw);

		clamp(_target_delta, -PI_DIV_4, PI_DIV_4);
	}
	
	SetState(state);
}

void __stdcall CAI_Boar::BoneCallback(CBoneInstance *B)
{
	CAI_Boar	*P = static_cast<CAI_Boar*>(B->Callback_Param);

	if (!P->look_at_enemy) return;
	
	Fmatrix M;
	M.setXYZi (P->_cur_delta,0.0f, 0.0f);
	B->mTransform.mulB(M);
}

BOOL CAI_Boar::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head"));
	BI.set_callback(BoneCallback,this);
	
	_cur_delta		= _target_delta = 0.f;
	_velocity		= PI;
	look_at_enemy	= false;
	return TRUE;
}


void CAI_Boar::LookPosition(Fvector to_point, float angular_speed)
{
	Fvector dir;
	dir.sub(to_point, Position());
	float yaw,pitch;
	dir.getHP(yaw,pitch);

	CMovementManager::m_body.target.yaw = angle_normalize(-yaw);
}


void CAI_Boar::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_ROTATION_JUMP) == ASP_ROTATION_JUMP) {
		float yaw, pitch;
		Fvector().sub(EnemyMan.get_enemy()->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;
		yaw = angle_normalize(yaw);

		EMotionAnim anim = eAnimJumpLeft;
		if (from_right(yaw,m_body.current.yaw)) {
			anim = eAnimJumpRight;
			yaw = angle_normalize(yaw + PI / 20);	
		} else yaw = angle_normalize(yaw - PI / 20);

		MotionMan.Seq_Add(anim);
		MotionMan.Seq_Switch();

		CMovementManager::m_body.target.yaw = yaw;

		// calculate angular speed
		float new_angular_velocity; 
		float delta_yaw = angle_difference(yaw,m_body.current.yaw);
		float time = MotionMan.GetCurAnimTime();
		new_angular_velocity = 2.5f * delta_yaw / time; 

		MotionMan.ForceAngularSpeed(new_angular_velocity);

		return;
	}

	if ((spec_params & ASP_ATTACK_RUN) == ASP_ATTACK_RUN) {
		MotionMan.SetCurAnim(eAnimAttackRun);
	}
}

void CAI_Boar::UpdateCL()
{
	inherited::UpdateCL();
	angle_lerp(_cur_delta, _target_delta, _velocity, Device.fTimeDelta);
}


void CAI_Boar::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) {
		//m_body.current.yaw = m_body.target.yaw;
		return;
	}
	
	EMotionAnim anim = MotionMan.GetCurAnim();
	
	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 
 
	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		case eAnimJumpLeft:
		case eAnimJumpRight:
			MotionMan.SetCurAnim(anim);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}

}
