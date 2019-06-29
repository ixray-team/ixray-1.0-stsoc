////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 21.06.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"

#define TORSO_ANGLE_DELTA		(PI/30.f)

// bones
void CAI_Soldier::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone = PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_head"));
	PKinematics(Visual())->LL_GetBoneInstance(u16(head_bone)).set_callback(HeadSpinCallback,this);
	
//	int legs_bone = PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_legs"));
//	PKinematics(Visual())->LL_GetBoneInstance(u16(head_bone)).set_callback(LegsSpinCallback,this);
}

void __stdcall CAI_Soldier::HeadSpinCallback(CBoneInstance* B)
{
	CAI_Soldier*		A = dynamic_cast<CAI_Soldier*> (static_cast<CObject*>(B->Callback_Param));
	
	Fmatrix				spin;
	spin.setXYZ			(-(A->r_current.yaw - A->r_torso_current.yaw), A->r_current.pitch - A->r_torso_current.pitch, 0 );
	B->mTransform.mulB_43(spin);
}

void __stdcall CAI_Soldier::LegsSpinCallback(CBoneInstance* B)
{
//	CAI_Soldier*		A = dynamic_cast<CAI_Soldier*> (static_cast<CObject*>(B->Callback_Param));
	
//	Fmatrix				spin;
//	float				bone_yaw	= A->r_torso_current.yaw - A->r_model_yaw - A->r_model_yaw_delta;
//	float				bone_pitch	= A->r_torso_current.pitch;
//	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
//	spin.setXYZ			(bone_yaw,bone_pitch,0);
//	B->mTransform.mulB_43(spin);
}

// sounds
void CAI_Soldier::vfLoadSounds()
{
	::Sound->create(sndHit[0],TRUE,"monsters\\Soldier\\Injured\\01",SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create(sndHit[1],TRUE,"monsters\\Soldier\\Injured\\02",SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create(sndHit[2],TRUE,"monsters\\Soldier\\Injured\\03",SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create(sndHit[3],TRUE,"monsters\\Soldier\\Injured\\04",SOUND_TYPE_MONSTER_INJURING_HUMAN);
	::Sound->create(sndDie[0],TRUE,"monsters\\Soldier\\Injured\\05",SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create(sndDie[1],TRUE,"monsters\\Soldier\\Injured\\06",SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create(sndDie[2],TRUE,"monsters\\Soldier\\Injured\\07",SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create(sndDie[3],TRUE,"monsters\\Soldier\\Injured\\08",SOUND_TYPE_MONSTER_DYING_HUMAN);
	::Sound->create(sndRadio[0],TRUE,"monsters\\Soldier\\Radio\\vo1",SOUND_TYPE_MONSTER_TALKING_HUMAN);
	::Sound->create(sndRadio[1],TRUE,"monsters\\Soldier\\Radio\\vo2",SOUND_TYPE_MONSTER_TALKING_HUMAN);
	::Sound->create(sndRadio[2],TRUE,"monsters\\Soldier\\Radio\\vo3",SOUND_TYPE_MONSTER_TALKING_HUMAN);
	::Sound->create(sndRadio[3],TRUE,"monsters\\Soldier\\Radio\\R1",SOUND_TYPE_MONSTER_TALKING_HUMAN);
//	if (g_Squad() == MEGAPHONE_SQUAD) {
//		::Sound->destroy(sndRadio[0]);
//		Sound->create(sndRadio[0],TRUE,"monsters\\Soldier\\Radio\\megafon_1m",0,SOUND_TYPE_MONSTER_TALKING_HUMAN);
//	}
	::Sound->create(sndSteps[0],TRUE,"Actor\\StepL",SOUND_TYPE_MONSTER_WALKING_HUMAN);
	::Sound->create(sndSteps[1],TRUE,"Actor\\StepR",SOUND_TYPE_MONSTER_WALKING_HUMAN);
}	

// animations
void CAI_Soldier::vfLoadAnimations()
{
	CSkeletonAnimated* tpVisualObject = PSkeletonAnimated(Visual());
	
	// loading normal animations
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle("norm_death_0");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle("norm_death_1");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[2] = tpVisualObject->ID_Cycle("norm_death_2");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[3] = tpVisualObject->ID_Cycle("norm_death_3");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[4] = tpVisualObject->ID_Cycle("norm_death_4");
	
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[0] = tpVisualObject->ID_Cycle("norm_run_fwd_0");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[1] = tpVisualObject->ID_Cycle("norm_run_fwd_1");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[2] = tpVisualObject->ID_Cycle("norm_run_fwd_2");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[3] = tpVisualObject->ID_Cycle("norm_run_fwd_3");

	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle("norm_walk_fwd_0");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle("norm_walk_fwd_1");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2] = tpVisualObject->ID_Cycle("norm_walk_fwd_2");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[3] = tpVisualObject->ID_Cycle("norm_walk_fwd_3");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[4] = tpVisualObject->ID_Cycle("norm_walk_fwd_4");
	
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle("norm_walk_back_0");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle("norm_walk_back_1");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[2] = tpVisualObject->ID_Cycle("norm_walk_back_2");
	
	tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0] = tpVisualObject->ID_Cycle("norm_lie_down_0");
	tSoldierAnimations.tNormal.tGlobal.tpaLieDown[1] = tpVisualObject->ID_Cycle("norm_lie_down_1");
	tSoldierAnimations.tNormal.tGlobal.tpaLieDown[2] = tpVisualObject->ID_Cycle("norm_lie_down_2");

	tSoldierAnimations.tNormal.tGlobal.tpaIdle[0] = tpVisualObject->ID_Cycle("norm_idle");
	tSoldierAnimations.tNormal.tGlobal.tpaIdle[1] = tpVisualObject->ID_Cycle("norm_idle_1");

	tSoldierAnimations.tNormal.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("norm_walk_ls");
	tSoldierAnimations.tNormal.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("norm_walk_rs");
	
	tSoldierAnimations.tNormal.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("norm_turn_ls");
	tSoldierAnimations.tNormal.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("norm_turn_rs");
	
	tSoldierAnimations.tNormal.tGlobal.tpPointSign = tpVisualObject->ID_Cycle("norm_sign_2");

	tSoldierAnimations.tNormal.tGlobal.tpAim = tpVisualObject->ID_Cycle("norm_aim");
	tSoldierAnimations.tNormal.tGlobal.tpAttack = tpVisualObject->ID_Cycle("norm_attack");
	tSoldierAnimations.tNormal.tGlobal.tpReload = tpVisualObject->ID_Cycle("norm_reload");

	tSoldierAnimations.tNormal.tTorso.tpDamageLeft = tpVisualObject->ID_FX("norm_damage_ls");
	tSoldierAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");
	tSoldierAnimations.tNormal.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle("norm_sign_0");
	tSoldierAnimations.tNormal.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle("norm_sign_1");
	
	tSoldierAnimations.tNormal.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle("norm_jump_begin");
	tSoldierAnimations.tNormal.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle("norm_jump_idle");
	
	// loading crouch animations
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle("cr_walk_fwd_0");
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle("cr_walk_fwd_1");
	
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle("cr_walk_back_0");
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle("cr_walk_back_1");
	
	tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0] = tpVisualObject->ID_Cycle("cr_lie_down_0");
	tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[1] = tpVisualObject->ID_Cycle("cr_lie_down_1");

	tSoldierAnimations.tCrouch.tGlobal.tpDeath = tpVisualObject->ID_Cycle("cr_death_0");
	tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("cr_turn_ls");
	tSoldierAnimations.tCrouch.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("cr_turn_rs");
	tSoldierAnimations.tCrouch.tGlobal.tpIdle = tpVisualObject->ID_Cycle("cr_idle");
	tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("cr_walk_ls");
	tSoldierAnimations.tCrouch.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("cr_walk_rs");
	tSoldierAnimations.tCrouch.tGlobal.tpPointSign = tpVisualObject->ID_Cycle("cr_sign_2");

	tSoldierAnimations.tCrouch.tGlobal.tpAim = tpVisualObject->ID_Cycle("cr_aim");
	tSoldierAnimations.tCrouch.tGlobal.tpAttack = tpVisualObject->ID_Cycle("cr_attack");
	tSoldierAnimations.tCrouch.tGlobal.tpReload = tpVisualObject->ID_Cycle("cr_reload");
	tSoldierAnimations.tCrouch.tTorso.tpDamageLeft = tpVisualObject->ID_FX("cr_damage_ls");
	tSoldierAnimations.tCrouch.tTorso.tpDamageRight = tpVisualObject->ID_FX("cr_damage_rs");
	tSoldierAnimations.tCrouch.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle("cr_sign_0");
	tSoldierAnimations.tCrouch.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle("cr_sign_1");

	tSoldierAnimations.tCrouch.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle("cr_jump_begin");
	tSoldierAnimations.tCrouch.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle("cr_jump_idle");

	// loading lie animations
	tSoldierAnimations.tLie.tGlobal.tpDeath = tpVisualObject->ID_Cycle("lie_death");
	tSoldierAnimations.tLie.tGlobal.tpStandUp = tpVisualObject->ID_Cycle("lie_stand_up");
	tSoldierAnimations.tLie.tGlobal.tpIdle = tpVisualObject->ID_Cycle("lie_idle");
	tSoldierAnimations.tLie.tGlobal.tpReload = tpVisualObject->ID_Cycle("lie_reload");
	tSoldierAnimations.tLie.tGlobal.tpAttack = tpVisualObject->ID_Cycle("lie_attack");
	tSoldierAnimations.tLie.tGlobal.tpDamage = tpVisualObject->ID_FX("lie_damage");
	tSoldierAnimations.tLie.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("lie_turn_ls");
	tSoldierAnimations.tLie.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("lie_turn_rs");
	tSoldierAnimations.tLie.tGlobal.tpWalkForward = tpVisualObject->ID_Cycle("lie_walk_fwd");
	tSoldierAnimations.tLie.tGlobal.tpWalkBack = tpVisualObject->ID_Cycle("lie_walk_back");
	tSoldierAnimations.tLie.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("lie_roll_ls");
	tSoldierAnimations.tLie.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("lie_roll_rs");
	//tSoldierAnimations.tLie.tGlobal.tpRaiseHandSign = tpVisualObject->ID_Cycle("lie_sign_0");
	//tSoldierAnimations.tLie.tGlobal.tpGoAheadSign = tpVisualObject->ID_Cycle("lie_sign_1");
	//tSoldierAnimations.tLie.tGlobal.tpPointSign = tpVisualObject->ID_Cycle("lie_sign_2");

	ZeroMemory				(m_tpaMovementAnimations,sizeof(m_tpaMovementAnimations));
	
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_0] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_1] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_2] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_3] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[3];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_4] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[4];
//	
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_0] = tSoldierAnimations.tNormal.tGlobal.tpaRunForward[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_1] = tSoldierAnimations.tNormal.tGlobal.tpaRunForward[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_2] = tSoldierAnimations.tNormal.tGlobal.tpaRunForward[2];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_3] = tSoldierAnimations.tNormal.tGlobal.tpaRunForward[3];
//	
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_0] = tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_1] = tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_2] = tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[2];
//
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_LEFT] = tSoldierAnimations.tNormal.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_RIGHT] = tSoldierAnimations.tNormal.tGlobal.tpWalkRight;
//
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_FORWARD_0] = tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[0];
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_FORWARD_1] = tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[1];
//	
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_BACK_0] = tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[0];
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_BACK_1] = tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[1];
//
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_LEFT] = tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_RIGHT] = tSoldierAnimations.tCrouch.tGlobal.tpWalkRight;
//
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_FORWARD_0] = tSoldierAnimations.tLie.tGlobal.tpWalkForward;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_BACK_0] = tSoldierAnimations.tLie.tGlobal.tpWalkBack;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_LEFT] = tSoldierAnimations.tLie.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_RIGHT] = tSoldierAnimations.tLie.tGlobal.tpWalkRight;

	m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaIdle[1]);
}

//static void __stdcall vfPlayCallBack(CBlend* B)
//{
//	CAI_Soldier* tpSoldier = (CAI_Soldier*)B->CallbackParam;
//	tpSoldier->m_bActionStarted = false;
//}
//
void CAI_Soldier::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
//	//R_ASSERT(fsimilar(_view.magnitude(),1));
//	//R_ASSERT(fsimilar(_move.magnitude(),1));
//
//	CKinematics* tpVisualObject = PKinematics(Visual());
//	
//	CMotionDef*	tpLegsAnimation=0;
//	CMotionDef*	tpTorsoAnimation=0;
//	CMotionDef*	tpGlobalAnimation=0;
//
//	if (g_Health() <= 0) {
////		switch (m_cBodyState) {
////			case BODY_STATE_STAND : {
////				for (int i=0 ;i<5; i++)
////					if (tSoldierAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
////						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
////						break;
////					}
////				if (!tpGlobalAnimation)
////					tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,5)];
////				break;
////			}
////			case BODY_STATE_CROUCH : {
////				//tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpDeath;
////				for (int i=0 ;i<5; i++)
////					if (tSoldierAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
////						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
////						break;
////					}
////				if (!tpGlobalAnimation)
////					tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,5)];
////				break;
////			}
////			case BODY_STATE_LIE : {
////				tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpDeath;
////				break;
////			}
////		}
//	}
//	else
//		/**
//		tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0];
//
//	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
//		//Msg("restarting animation..."); 
//		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
//		if (tpGlobalAnimation) {
//			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
//		}
//	}
//		/**/
//		switch (m_eCurrentState) {
//			case aiSoldierWaitForAnimation : {
//				tpGlobalAnimation = m_tpAnimationBeingWaited;
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
////			case aiSoldierJumping : {
////				switch (m_cBodyState) {
////					case BODY_STATE_STAND : {
////						if ((m_tpCurrentLegsAnimation != tSoldierAnimations.tNormal.tLegs.tpJumpBegin) && (m_tpCurrentLegsAnimation != tSoldierAnimations.tNormal.tLegs.tpJumpIdle)) {
////							tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpJumpBegin;
////							//Msg("Jump begin");
////						}
////						else
////							//if ((ps_Size() < 2) || (ps_Element(ps_Size() - 1).Position().y - ps_Element(ps_Size() - 2).Position().y > -EPS_L)) {
////							if (m_tpCurrentLegsBlend->playing) {
////								tpLegsAnimation = m_tpCurrentLegsAnimation;
////								//Msg("Jump continueing");
////							}
////							else {
////								tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpJumpIdle;
////								//Msg("Jump idle");
////							}
////						break;
////					}
////					case BODY_STATE_CROUCH : {
////						if ((m_tpCurrentLegsAnimation != tSoldierAnimations.tCrouch.tLegs.tpJumpBegin) && (m_tpCurrentLegsAnimation != tSoldierAnimations.tCrouch.tLegs.tpJumpIdle))
////							tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpJumpBegin;
////						else
////							if (m_tpCurrentLegsBlend->playing)
////								tpLegsAnimation = m_tpCurrentLegsAnimation;
////							else
////								tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpJumpIdle;
////						break;
////												}
////					case BODY_STATE_LIE : {
////						tpLegsAnimation = tSoldierAnimations.tLie.tGlobal.tpWalkForward;
////						break;
////					}
////				}
////				break;
////			}
///**/
//			case aiSoldierAttackAloneFireFire : 
//			case aiSoldierAttackGroupFireFire : {
//				if (m_bFiring)
//					;
////					switch (m_cBodyState) {
////						case BODY_STATE_STAND : {
////							tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAttack;
////							break;
////						}
////						case BODY_STATE_CROUCH : {
////							tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAttack;
////							break;
////						}
////						case BODY_STATE_LIE : {
////							tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpAttack;
////							break;
////						}
////					}
//				else
//					;
////					if (speed < EPS_L)
////						switch (m_cBodyState) {
////							case BODY_STATE_STAND : {
////								tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAim;
////								break;
////							}
////							case BODY_STATE_CROUCH : {
////								tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAim;
////								break;
////							}
////							case BODY_STATE_LIE : {
////								tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
////								break;
////							}
////						}
////					else
////						tpGlobalAnimation = m_tpaMovementAnimations[m_cBodyState][m_cMovementType];
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
//			case aiSoldierRecharge : {
////				switch (m_cBodyState) {
////					case BODY_STATE_STAND : {
////						tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpReload;
////						break;
////					}
////					case BODY_STATE_CROUCH : {
////						tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpReload;
////						break;
////					}
////					case BODY_STATE_LIE : {
////						tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpReload;
////						break;
////					}
////				}
//				break;
//			}
///**
//			case aiSoldierAttackAim : {
//				switch (m_cBodyState) {
//					case BODY_STATE_STAND : {
//						tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAim;
//						break;
//					}
//					case BODY_STATE_CROUCH : {
//						tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAim;
//						break;
//					}
//					case BODY_STATE_LIE : {
//						tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
//						break;
//					}
//				}
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
//			case aiSoldierPointAtSmth : {
//				switch (m_cBodyState) {
//					case BODY_STATE_STAND : {
//						tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpPointSign;
//						break;
//					}
//					case BODY_STATE_CROUCH : {
//						tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpPointSign;
//						break;
//					}
//					case BODY_STATE_LIE : {
//						//tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpPointSign;
//						break;
//					}
//				}
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
///**/
//			default : {
//				// not moving
//				//if (speed < .1f) {
////				Msg("speed %.2f",speed);
////				Msg("m_fCurSpeed %.2f",m_fCurSpeed);
////				Msg("BODY_STATE %d",m_cBodyState);
//				if (speed < .1f) {
//					// turning around || standing idle
//					if (_abs(r_torso_target.yaw - r_torso_current.yaw) <= PI)
//						if (_abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
//							if (r_torso_target.yaw - r_torso_current.yaw >= 0)
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnRight;
//										break;
//									}
//								}
//							else
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnLeft;
//										break;
//									}
//								}
//						else
////							switch (m_eCurrentState) {
////								/**
////								case aiSoldierHurtAloneDefend : {
////									switch (m_cBodyState) {
////										case BODY_STATE_STAND : {
////											tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAim;
////											break;
////										}
////										case BODY_STATE_CROUCH : {
////											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAim;
////											break;
////										}
////										case BODY_STATE_LIE : {
////											tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
////											break;
////										}
////									}
////									tpTorsoAnimation = tpLegsAnimation = 0;
////									break;
////								}
////								/**/
////								default : {
////									/**
////									if ((m_eCurrentState == aiSoldierHurtAloneDefend) && (!AI_Path.TravelPath.size() || (AI_Path.TravelPath.size() - 1 > AI_Path.TravelStart))) {
////										switch (m_cBodyState) {
////											case BODY_STATE_STAND : {
////												tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAim;
////												break;
////											}
////											case BODY_STATE_CROUCH : {
////												tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAim;
////												break;
////											}
////											case BODY_STATE_LIE : {
////												tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
////												break;
////											}
////										}
////										tpTorsoAnimation = tpLegsAnimation = 0;
////									}
////									else /**/{
////										switch (m_cBodyState) {
////											case BODY_STATE_STAND : {
////												tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaIdle[1];
////												break;
////											}
////											case BODY_STATE_CROUCH : {
////												tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpIdle;
////												break;
////											}
////											case BODY_STATE_LIE : {
////												tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
////												break;
////											}
////										}
////										break;
////									}
////								}
////							}
//						switch (m_cBodyState) {
//							case BODY_STATE_STAND : {
//								tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaIdle[1];
//								break;
//							}
//							case BODY_STATE_CROUCH : {
//								tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpIdle;
//								break;
//							}
//							case BODY_STATE_LIE : {
//								tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
//								break;
//							}
//						}
//					else
//						if (PI_MUL_2 - _abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
//							if (r_torso_target.yaw > r_torso_current.yaw)
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnLeft;
//										break;
//									}
//								}
//							else
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnRight;
//										break;
//									}
//								}
//						else
//							/**
//							if ((m_eCurrentState == aiSoldierHurtAloneDefend) && (!AI_Path.TravelPath.size() || (AI_Path.TravelPath.size() - 1 > AI_Path.TravelStart))) {
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpAim;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpAim;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
//										break;
//									}
//								}
//								tpTorsoAnimation = tpLegsAnimation = 0;
//							}
//							else /**/{
////								switch (m_cBodyState) {
////									case BODY_STATE_STAND : {
////										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaIdle[1];
////										break;
////									}
////									case BODY_STATE_CROUCH : {
////										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpIdle;
////										break;
////									}
////									case BODY_STATE_LIE : {
////										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
////										break;
////									}
////								}
//								break;
//							}
//					// torso
//				}
//				else {
//					//Msg("moving...");
//					tpGlobalAnimation = m_tpaMovementAnimations[m_cBodyState][m_cMovementType];
//					if (!tpGlobalAnimation) {
//						tpGlobalAnimation = tpGlobalAnimation;
//					}
//					//torso
//				}
//			}
//		}
//	
//	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
//		//Msg("restarting animation..."); 
//		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
//		if (tpGlobalAnimation) {
//			if (m_eCurrentState == aiSoldierWaitForAnimation)
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation,TRUE,vfPlayCallBack,this);
//			else
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
//		}
//	}
//
//	/**
//	if (tpTorsoAnimation != m_tpCurrentTorsoAnimation) { 
//		//Msg("restarting animation..."); 
//		m_tpCurrentTorsoAnimation = tpTorsoAnimation;
//		if (tpTorsoAnimation) {
//			m_tpCurrentTorsoBlend = tpVisualObject->PlayCycle(tpTorsoAnimation);
//		}
//	}
//
//	if (tpLegsAnimation != m_tpCurrentLegsAnimation) { 
//		//Msg("restarting animation..."); 
//		m_tpCurrentLegsAnimation = tpLegsAnimation;
//		if (tpLegsAnimation) {
//			m_tpCurrentLegsBlend = tpVisualObject->PlayCycle(tpLegsAnimation);
//		}
//	}
//	/**/
}
