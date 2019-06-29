#include "stdafx.h"
#include "ai_pseudodog.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_effector.h"
#include "../../actor.h"
#include "../../ActorEffector.h"
#include "../stalker/ai_stalker.h"
#include "pseudodog_state_growling.h"
#include "../../../CameraBase.h"
#include "../../xr_level_controller.h"


CAI_PseudoDog::CAI_PseudoDog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingRunAway>	(this);
	stateGrowling		= xr_new<CPseudodogGrowling>(this);

	CurrentState		= stateRest;
	CurrentState->Reset	();

	CJumping::Init		(this);
}

CAI_PseudoDog::~CAI_PseudoDog()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateExploreDNE);
	xr_delete(stateGrowling);

}

void CAI_PseudoDog::reinit()
{
	inherited::reinit();

	strike_in_jump					= false;
	m_time_became_angry				= 0;
	time_growling					= 0;
}

void CAI_PseudoDog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);


	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);


	// Load psi postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "psi_effector");
	m_psi_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_psi_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_psi_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_psi_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_psi_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_psi_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_psi_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_psi_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_psi_effector.ppi.color_base.r,	&m_psi_effector.ppi.color_base.g,	&m_psi_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_psi_effector.ppi.color_gray.r,	&m_psi_effector.ppi.color_gray.g,	&m_psi_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_psi_effector.ppi.color_add.r,	&m_psi_effector.ppi.color_add.g,	&m_psi_effector.ppi.color_add.b);

	m_psi_effector.time			= pSettings->r_float(ppi_section,"time");
	m_psi_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_psi_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_psi_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_psi_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_psi_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_psi_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------

	m_anger_hunger_threshold	= pSettings->r_float(section, "anger_hunger_threshold");
	m_anger_loud_threshold		= pSettings->r_float(section, "anger_loud_threshold");

	::Sound->create(psy_effect_sound,TRUE, pSettings->r_string(section,"sound_psy_effect"), SOUND_TYPE_WORLD);

	psy_effect_turn_angle		= angle_normalize(pSettings->r_float(section,"psy_effect_turn_angle"));

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
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);	
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT);
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimAttackPsi,		"stand_psi_attack_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
		MotionMan.AddAnim(eAnimThreaten,		"stand_howling_",		-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);


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
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft,	eAnimStandTurnRight, PI_DIV_6);
		MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);	
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

void CAI_PseudoDog::reload(LPCSTR section)
{
	inherited::reload	(section);
	
	CSoundPlayer::add	(pSettings->r_string(section,"sound_psy_attack"),	16,	SOUND_TYPE_MONSTER_ATTACKING,	1,	u32(1 << 31) | 15,	MonsterSpace::eMonsterSoundPsyAttack, "bip01_head");
	
	CJumping::AddState	(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_prepare_0"),	JT_CUSTOM,	true,	0.f, inherited::get_sd()->m_fsVelocityRunFwdNormal.velocity.angular_real);
	CJumping::AddState	(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_glide_0"),	JT_GLIDE,	false,	0.f, inherited::get_sd()->m_fsVelocityRunFwdNormal.velocity.angular_real);
}


#define MIN_ANGRY_TIME		10000
#define MAX_GROWLING_TIME	20000

void CAI_PseudoDog::StateSelector()
{	
	bool prev_angry = m_bAngry;
	m_bAngry = false;

	if (HitMemory.is_hit())																m_bAngry = true;
	if (SoundMemory.is_loud_sound(m_anger_loud_threshold) || hear_dangerous_sound)		m_bAngry = true;
	if (CEntityCondition::GetSatiety() < m_anger_hunger_threshold)						m_bAngry = true;
	
	// если слишком долго рычит - включать универсальную схему
	if ((time_growling !=0) && (time_growling + MAX_GROWLING_TIME < m_current_update))	m_bAngry = true;

	// если на этом кадре стал злым, сохранить время когда стал злым
	if ((prev_angry == false) && m_bAngry) m_time_became_angry = m_current_update;
	if (!m_bAngry && (m_time_became_angry + MIN_ANGRY_TIME > m_current_update))			m_bAngry = true;
	
	const CEntityAlive	*enemy		= EnemyMan.get_enemy();
	const CAI_Stalker	*pStalker	= smart_cast<const CAI_Stalker *>(enemy);
	const CActor		*pActor		= smart_cast<const CActor *>(enemy);

	TTime last_hit_time = 0;
	if (HitMemory.is_hit()) last_hit_time = HitMemory.get_last_hit_time();

	if (!m_bAngry && (pActor || pStalker)) {
		if (EnemyMan.get_danger_type() == eVeryStrong) SetState(statePanic);
		else SetState(stateGrowling);
	} else {
		// если злой или враг - монстр
		if (enemy) {
			switch (EnemyMan.get_danger_type()) {
				case eVeryStrong:	SetState(statePanic); break;
				case eStrong:		
				case eNormal:
				case eWeak:			SetState(stateAttack); break;
			}
	} else if (HitMemory.is_hit() && (last_hit_time + 10000 > m_current_update)) SetState(stateExploreDNE);
	else if (hear_dangerous_sound || hear_interesting_sound) {
			if (hear_dangerous_sound)			SetState(stateExploreNDE);		
			if (hear_interesting_sound)			SetState(stateExploreNDE);	
		} else if (CorpseMan.get_corpse() && ((GetSatiety() < get_sd()->m_fMinSatiety) || flagEatNow))
			SetState(stateEat);	
		else									SetState(stateRest);
	}

	if ((time_growling == 0) && (CurrentState == stateGrowling)) time_growling = m_current_update;
	if ((CurrentState != stateGrowling)) time_growling = 0;

}

void CAI_PseudoDog::UpdateCL()
{
	inherited::UpdateCL	();
	CJumping::Update	();

	float trace_dist = 1.5f;
	
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
				SAAParam params;
				MotionMan.AA_GetParams(params, "jump_glide_0");
				HitEntity(pE, params.hit_power, params.impulse, params.impulse_dir);
				strike_in_jump = true;
			}
		}
		setEnabled(true);			
	}

}
void CAI_PseudoDog::OnJumpStop()
{
	MotionMan.DeactivateJump();
	strike_in_jump = false;
}


void CAI_PseudoDog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_PSI_ATTACK) == ASP_PSI_ATTACK) {
		MotionMan.Seq_Add(eAnimAttackPsi);
		MotionMan.Seq_Switch();

		CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
		if (pA) {
			pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_psi_effector.ce_time,m_psi_effector.ce_amplitude,m_psi_effector.ce_period_number,m_psi_effector.ce_power));
			Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_psi_effector.ppi, m_psi_effector.time, m_psi_effector.time_attack, m_psi_effector.time_release));

			if (pA->cam_Active()) {
				pA->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, Random.randF(psy_effect_turn_angle)); 
				pA->cam_Active()->Move(Random.randI(2) ? kUP	: kDOWN, Random.randF(psy_effect_turn_angle)); 
			}

		}
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
	}
}


void CAI_PseudoDog::play_effect_sound()
{
	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;
	
	Fvector pos = pA->Position();
	pos.y += 1.5f;
	::Sound->play_at_pos(psy_effect_sound,pA,pos);
}

