#include "stdafx.h"
#include "ai_flesh.h"
#include "../../ai_space.h"
#include "../ai_monster_utils.h"

CAI_Flesh::CAI_Flesh()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreDNE		= xr_new<CBitingRunAway>	(this);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateSearchEnemy	= xr_new<CBitingSearchEnemy>(this);
	stateControlled		= xr_new<CBitingControlled>	(this);

	CurrentState		= stateRest;
	CurrentState->Reset();
	
	m_fEyeShiftYaw		= PI_DIV_6;

	controlled::init_external(this);
}

CAI_Flesh::~CAI_Flesh()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreDE);
	xr_delete(stateExploreNDE);
	xr_delete(stateControlled);
	xr_delete(stateSearchEnemy);
}

BOOL CAI_Flesh::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	return TRUE;
}

void CAI_Flesh::Load(LPCSTR section)
{
	inherited::Load(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		// define animation set
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttackRat,		"stand_attack_rat_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttackFromBack,	"stand_attack_back_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_eat_",			 1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimSteal,			"stand_crawl_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		// define transitions
		MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
		MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);

		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/10);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_2);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimScared);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();

	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}


void CAI_Flesh::StateSelector()
{
	if (is_under_control()) {
		SetState(stateControlled);
		return;
	}
	
	
	TTime last_hit_time = 0;
	if (HitMemory.is_hit()) last_hit_time = HitMemory.get_last_hit_time();

	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong:	SetState(statePanic); break;
			case eStrong:		
			case eNormal:
			case eWeak:			SetState(stateAttack); break;
		}
	} else if (HitMemory.is_hit() && (last_hit_time + 10000 > m_current_update)) SetState(stateExploreDNE);
	else if (hear_dangerous_sound || hear_interesting_sound) {
			if (hear_dangerous_sound)			SetState(stateExploreDNE);		
		if (hear_interesting_sound)			SetState(stateExploreNDE);	
	} else if (CorpseMan.get_corpse() && ((GetSatiety() < get_sd()->m_fMinSatiety) || flagEatNow))					
											SetState(stateEat);	
	else									SetState(stateRest);

}


// возвращает true, если после выполнения этой функции необходимо прервать обработку
// т.е. если активирована последовательность
void CAI_Flesh::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) MotionMan.SetCurAnim(eAnimDragCorpse);

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_BACK_ATTACK) == ASP_BACK_ATTACK) {
		MotionMan.Seq_Add(eAnimAttackFromBack);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_ATTACK_RAT) == ASP_ATTACK_RAT) MotionMan.SetCurAnim(eAnimAttackRat);
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) MotionMan.SetCurAnim(eAnimThreaten);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Функция ConeSphereIntersection
// Пересечение конуса (не ограниченного) со сферой
// Необходима для определения пересечения копыта плоти с баунд-сферой крысы
// Параметры: ConeVertex - вершина конуса, ConeAngle - угол конуса (между поверхностью и высотой)
// ConeDir - направление конуса, SphereCenter - центр сферы, SphereRadius - радиус сферы
bool CAI_Flesh::ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius)
{
	float fInvSin = 1.0f/_sin(ConeAngle);
	float fCosSqr = _cos(ConeAngle)*_cos(ConeAngle);

	
	Fvector kCmV;	kCmV.sub(SphereCenter,ConeVertex);
	Fvector kD		= kCmV;
	Fvector tempV	= ConeDir;
	tempV.mul		(SphereRadius* fInvSin);
	kD.add			(tempV);

	float fDSqrLen = kD.square_magnitude();
	float fE = kD.dotproduct(ConeDir);
	if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr ) {
		
		float fSinSqr = _sin(ConeAngle)*_sin(ConeAngle);

		fDSqrLen = kCmV.square_magnitude();
		fE = -kCmV.dotproduct(ConeDir);
		if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
			float fRSqr = SphereRadius*SphereRadius;
			return fDSqrLen <= fRSqr;
		} else return true;
	} 
	
	return false;
}
