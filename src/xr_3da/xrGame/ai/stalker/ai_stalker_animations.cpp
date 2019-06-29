////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_animations.h"
#include "../../../motion.h"
#include "../../weapon.h"
#include "../../missile.h"
#include "../../script_entity_action.h"
#include "../../inventory.h"
#include "../../fooditem.h"
#include "../../clsid_game.h"
#include "../../../skeletonanimated.h"

static const float y_spin_factor			= 0.25f;
static const float y_shoulder_factor		= 0.25f;
static const float y_head_factor			= 0.5f;
static const float p_spin_factor			= 0.2f;
static const float p_shoulder_factor		= 0.7f;
static const float p_head_factor			= 0.1f;

static const float y_spin_fire_factor		= 0.5f;
static const float y_shoulder_fire_factor	= 0.5f;
static const float y_head_fire_factor		= 0.0f;
static const float p_spin_fire_factor		= 0.2f;
static const float p_shoulder_fire_factor	= 0.7f;
static const float p_head_fire_factor		= 0.1f;

float faTurnAngles			[] = {
	0.f,
	M_PI,
	PI_DIV_6,
	0.f,
};

LPCSTR caStateNames			[] = {
	"cr_",
	"norm_",
	"dmg_norm_",
	0
};

LPCSTR caWeaponNames		[] = {
	"0_",
	"1_",
	"2_",
	"3_",
	"4_",
	"5_",
	"6_",
	"7_",
	0
};

LPCSTR caWeaponActionNames	[] = {
	"draw_",		// 0
	"attack_",		// 1
	"drop_",		// 2
	"holster_",		// 3
	"reload_",		// 4
	"pick_",		// 5
	"aim_",			// 6
	"walk_",		// 7
	"run_",			// 8
	"idle_",		// 9
	"escape_",		// 10
	"prepare_",		// 11
	"playing",		// 12
	0
};

LPCSTR caMovementNames		[] = {
	"walk_",
	"run_",
	0
};

LPCSTR caMovementActionNames[] = {
	"fwd_",
	"back_",
	"ls_",
	"rs_",
	0
};

LPCSTR caInPlaceNames		[] = {
	"idle_1",
	"turn_left_0",
	"turn_right_0",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
	0
};

LPCSTR caGlobalNames		[] = {
	"damage_",
	"escape_",
	"dead_stop_",
	0
};

LPCSTR caHeadNames			[] = {
	"head_idle_0",
	"head_talk_0",
	0
};

CStateAnimations::CStateAnimations	()
{
	m_tInPlace	= xr_new<CAniFVector<caInPlaceNames> >();
}

CStateAnimations::CStateAnimations	(const CStateAnimations &anims)
{
	clone		(anims.m_tInPlace,m_tInPlace);
}

CStateAnimations::~CStateAnimations	()
{
	xr_delete	(m_tInPlace);
}

void CStateAnimations::Load			(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName)
{
	string256			S;
	m_tGlobal.Load		(tpKinematics,caBaseName);
	m_tTorso.Load		(tpKinematics,strconcat(S,caBaseName,"torso_"));
	m_tMoves.Load		(tpKinematics,caBaseName);
	m_tInPlace->Load	(tpKinematics,caBaseName);
};

void __stdcall CStalkerAnimations::HeadCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_head_fire_factor;
		pitch_factor		= p_head_fire_factor;
	}
	else {
		yaw_factor			= y_head_factor;
		pitch_factor		= p_head_factor;
	}
//	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->head_orientation().current.yaw - (A->body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));

	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
//	B->mTransform.setXYZ	(yaw, pitch, 0);
//	B->mTransform.c			= c;
}

void __stdcall CStalkerAnimations::ShoulderCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_shoulder_fire_factor;
		pitch_factor		= p_shoulder_fire_factor;
	}
	else {
		yaw_factor			= y_shoulder_factor;
		pitch_factor		= p_shoulder_factor;
	}
//	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->head_orientation().current.yaw - (A->body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CStalkerAnimations::SpinCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_spin_fire_factor;
		pitch_factor		= p_spin_fire_factor;
	}
	else {
		yaw_factor			= y_spin_factor;
		pitch_factor		= p_spin_factor;
	}
//	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->head_orientation().current.yaw - (A->body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void CStalkerAnimations::vfAssignGlobalAnimation(CMotionDef *&tpGlobalAnimation)
{
	if ((eMentalStatePanic == m_object->mental_state()) && (m_object->speed() > EPS_L)) {
		tpGlobalAnimation	= m_tAnims.A[m_object->IsLimping() ? eBodyStateStandDamaged : eBodyStateStand].m_tGlobal.A[1].A[0];
		return;
	}

	CFoodItem				*food_item = smart_cast<CFoodItem*>(m_object->inventory().ActiveItem());
	if (food_item) {
		u32					dwCurrentAniSlot = food_item->animation_slot();
		switch (food_item->STATE) {
			case FOOD_HIDDEN:
			case FOOD_SHOWING: {
//				Msg				("Showing vodka");
				tpGlobalAnimation = m_tGlobalItem.A[dwCurrentAniSlot].A[0].A[0];
				break;
			}
			case FOOD_HIDING : {
//				Msg				("Hiding vodka");
				tpGlobalAnimation = m_tGlobalItem.A[dwCurrentAniSlot].A[3].A[0];
				break;
			}
			case FOOD_PLAYING :
			case FOOD_IDLE	 : {
//				Msg				("Holding vodka");
				tpGlobalAnimation = m_tGlobalItem.A[dwCurrentAniSlot].A[6].A[0];
				break;
			}
			case FOOD_PREPARE: {
//				Msg				("Preparing vodka");
				tpGlobalAnimation = m_tGlobalItem.A[dwCurrentAniSlot].A[11].A[0];
				break;
			}
			case FOOD_EATING : {
//				Msg				("Drinking vodka");
				tpGlobalAnimation = m_tGlobalItem.A[dwCurrentAniSlot].A[1].A[0];
				break;
			}
			default			 : NODEFAULT;
		}
	}
}

void CStalkerAnimations::vfAssignHeadAnimation(CMotionDef *&tpHeadAnimation)
{
	if (m_object->active_sound_count(true))
		tpHeadAnimation		= m_tHead.A[1];
	else
		tpHeadAnimation		= m_tHead.A[0];
}

void CStalkerAnimations::vfAssignTorsoAnimation(CMotionDef *&tpTorsoAnimation)
{
	CWeapon					*tpWeapon	= smart_cast<CWeapon*>(m_object->inventory().ActiveItem());
	CMissile				*missile	= smart_cast<CMissile*>(m_object->inventory().ActiveItem());
	CLASS_ID				clsid = tpWeapon ? tpWeapon->SUB_CLS_ID : missile ? missile->SUB_CLS_ID : 0;
	u32						dwCurrentAniSlot = 0;
	if (tpWeapon || missile)
		switch (clsid) {
			case CLSID_OBJECT_W_VINTOREZ	:
			case CLSID_OBJECT_W_VAL			:
			case CLSID_OBJECT_W_GROZA		:
			case CLSID_OBJECT_W_M134		:
			case CLSID_OBJECT_W_FN2000		:
			case CLSID_OBJECT_W_AK74		:
			case CLSID_OBJECT_W_LR300		: 
			case CLSID_OBJECT_W_SVU			:
			case CLSID_OBJECT_W_SVD			: {
				dwCurrentAniSlot = 2;
				break;
			}
			case CLSID_OBJECT_W_WALTHER		:
			case CLSID_OBJECT_W_USP45		:
			case CLSID_OBJECT_W_HPSA		:
			case CLSID_OBJECT_W_PM			:
			case CLSID_OBJECT_W_FORT		: {
				dwCurrentAniSlot = 1;
				break;
			}
			case CLSID_OBJECT_W_BINOCULAR	: {
//				dwCurrentAniSlot = 5;
				dwCurrentAniSlot = 1;
				break;
			}
			case CLSID_OBJECT_W_SHOTGUN		: {
				dwCurrentAniSlot = 3;
				break;
			}
			case CLSID_OBJECT_W_RPG7		: {
				dwCurrentAniSlot = 2;
				break;
			}
			case CLSID_GRENADE_F1		:
			case CLSID_GRENADE_RGD5		: {
				dwCurrentAniSlot = 6;
				break;
			}
			case CLSID_IITEM_BOLT		: {
				dwCurrentAniSlot = 6;
				break;
			}
			default : {
#pragma todo("Dima to Dima : Return nodefault")
				dwCurrentAniSlot = 1;
				//NODEFAULT;
			}
		}

	
//	Msg			("[%s] Current weapon slot   : %d",*cName(),dwCurrentAniSlot);
//	Msg			("[%s] Current movement type : %d",*cName(),m_object->movement_type());

	EBodyState l_tBodyState = ((eBodyStateStand == m_object->body_state()) && m_object->IsLimping()) ? eBodyStateStandDamaged : m_object->body_state();
	if (m_object->inventory().ActiveItem()) {
		if (tpWeapon) {
			switch (tpWeapon->STATE) {
				case CWeapon::eReload : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Reload");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[4].A[0];
					break;
				}
				case CWeapon::eShowing : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Showing");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case CWeapon::eHiding : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Hiding");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case CWeapon::eFire: {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire1");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case CWeapon::eFire2 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire2");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
			}
		}
		
		if (missile && (m_object->body_state() == eBodyStateStand)) {
			switch (missile->State()) {
				case MS_SHOWING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Showing");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case MS_HIDING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Hiding");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case MS_THREATEN : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire start");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case MS_READY	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire idle");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[1];
					break;
				}
				case MS_THROW	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire end");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[2];
					break;
				}
				case MS_END		 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire smooth");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[2];
					break;
				}
				case MS_PLAYING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire end1");
					tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[2];
					break;
				}
				case MS_IDLE	 :
				case MS_HIDDEN	 :
				case MS_EMPTY	 :
				default			 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Aim");
					tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : 6].A[0];
					break;
				}
			}
		}
		else
			if ((m_object->body_state() == eBodyStateCrouch) && missile)
				dwCurrentAniSlot	= 0;

		if (!tpTorsoAnimation) {
			switch (m_object->CObjectHandler::current_action_state_id()) {
				case ObjectHandlerSpace::eWorldOperatorAim1 :
				case ObjectHandlerSpace::eWorldOperatorAim2 :
				case ObjectHandlerSpace::eWorldOperatorAimingReady1 :
				case ObjectHandlerSpace::eWorldOperatorAimingReady2 :
				case ObjectHandlerSpace::eWorldOperatorFire1 :
				case ObjectHandlerSpace::eWorldOperatorFire2 :
				{
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Aim");
					tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[((l_tBodyState == eBodyStateStand) && m_object->IsLimping()) ? 9 : 6].A[0];
					break;
				}
				default : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Idle");
					if (eMentalStateFree == m_object->mental_state()) {
						tpTorsoAnimation = 0;
						//. hack
//						R_ASSERT2(eBodyStateStand == m_object->body_state(),"Cannot run !free! animation when body state is not stand!");
						if (eBodyStateStand != m_object->body_state()) {
							m_object->set_body_state(eBodyStateStand);
							l_tBodyState	= eBodyStateStand;
						}
						if ((eMovementTypeStand == m_object->movement_type()) || fis_zero(m_object->speed()))
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[9].A[1];
						else
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : (7 + m_object->movement_type())].A[1];
//							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : (7 + m_object->movement_type())].A[m_object->IsLimping() ? 0 : 1];
						return;
					}
					else {
						if (fis_zero(m_object->speed())) {
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : 6].A[0];
							return;
						}
						switch (m_object->movement_type()) {
							case eMovementTypeStand : {
								tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : 6].A[0];
								break;
							}
							case eMovementTypeWalk : {
								if (m_object->body_state() == eBodyStateStand)
									tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[7].A[0];
								else
									tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[6].A[0];
								break;
							}
							case eMovementTypeRun : {
								if (m_object->body_state() == eBodyStateStand)
									tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 7 : 8].A[0];
								else
									tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[6].A[0];
								break;
							}
							default : NODEFAULT;
						}
					}
				}
			}
		}
	}
	else {
		if (eMentalStateFree == m_object->mental_state()) {
			tpTorsoAnimation = 0;
			R_ASSERT2(eBodyStateStand == m_object->body_state(),"Cannot run !free! animation when body state is not stand!");
			if ((eMovementTypeStand == m_object->movement_type()) || fis_zero(m_object->speed()))
				tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[9].A[0];
			else
				tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[m_object->IsLimping() ? 9 : (7 + m_object->movement_type())].A[m_object->IsLimping() ? 0 : 1];
		}
		else
			if ((eMovementTypeStand == m_object->movement_type()) || fis_zero(m_object->speed()))
				tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[0].A[6].A[0];
			else
				tpTorsoAnimation = m_tAnims.A[m_object->body_state()].m_tTorso.A[0].A[6].A[1];
	}
}

void CStalkerAnimations::vfAssignLegsAnimation(CMotionDef *&tpLegsAnimation)
{
	EBodyState				l_tBodyState = ((eBodyStateStand == m_object->body_state()) && m_object->IsLimping()) ? eBodyStateStandDamaged : m_object->body_state();
	
	// moving
	float					yaw, pitch;
	m_object->GetDirectionAngles	(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);

	if ((m_object->speed() < EPS_L) || (eMovementTypeStand == m_object->movement_type())) {
		// standing
		if (angle_difference(m_object->body_orientation().current.yaw,m_object->body_orientation().target.yaw) <= EPS_L) {
			tpLegsAnimation	= m_tAnims.A[m_object->body_state()].m_tInPlace->A[0];
//			Msg				("%6d : Playing animation standing",Level().timeServer());
//			Msg				("%6d : %f, %f",Level().timeServer(),((m_object->body_orientation().current.yaw)),((m_object->body_orientation().target.yaw)));
//			Msg				("%6d : %f, %f",Level().timeServer(),((m_object->head_orientation().current.yaw)),((m_object->head_orientation().target.yaw)));
		}
		else {
			tpLegsAnimation	= m_tAnims.A[m_object->body_state()].m_tInPlace->A[left_angle(-m_object->body_orientation().target.yaw,-m_object->body_orientation().current.yaw) ? 1 : 2];
//			Msg				("%6d : Playing animation turn %s",Level().timeServer(),left ? "LEFT" : "RIGHT");
//			Msg				("%6d : %f, %f",Level().timeServer(),((m_object->body_orientation().current.yaw)),((m_object->body_orientation().target.yaw)));
//			Msg				("%6d : %f, %f",Level().timeServer(),((m_object->head_orientation().current.yaw)),((m_object->head_orientation().target.yaw)));
		}
		return;
	}
//	float					fAngleDifference = _abs(angle_normalize_signed(m_object->body_orientation().current.yaw - m_object->head_orientation().current.yaw));
	float					fAnimationSwitchFactor = 1.f;//fAngleDifference < PI_DIV_2 ? 1.f : 1.f - (fAngleDifference - PI_DIV_2)/(PI - PI_DIV_2);
	if	(((eMovementDirectionForward == m_tDesirableDirection) && (eMovementDirectionBack == m_tMovementDirection))	||	((eMovementDirectionBack == m_tDesirableDirection) && (eMovementDirectionForward == m_tMovementDirection)))
		fAnimationSwitchFactor = .0f;

	if	(((eMovementDirectionRight == m_tDesirableDirection) && (eMovementDirectionLeft == m_tMovementDirection))	||	((eMovementDirectionLeft == m_tDesirableDirection) && (eMovementDirectionRight == m_tMovementDirection)))
		fAnimationSwitchFactor = .0f;

	if ((l_tBodyState != eBodyStateStand) && (eMentalStateDanger != m_object->mental_state()))
		m_object->set_mental_state	(eMentalStateDanger);

	if (eMentalStateDanger != m_object->mental_state())
		if (angle_difference(m_object->body_orientation().current.yaw,yaw) <= PI_DIV_6) {
			tpLegsAnimation = m_tAnims.A[l_tBodyState].m_tMoves.A[m_object->movement_type()].A[eMovementDirectionForward].A[m_object->mental_state()];
			return;
		}
		else {
			fAnimationSwitchFactor	= .0f;
//			m_object->m_body.speed	= PI_DIV_2;
//			m_object->m_head.speed	= 3*PI_DIV_2;
		}

	bool	forward_direction = false;
	bool	left = left_angle(-yaw,-m_object->head_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,m_object->head_orientation().current.yaw) <= 2*PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,m_object->head_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	if (forward_direction) {
//		Msg							("Moving FORWARD");
		// moving forward
		if (eMovementDirectionForward == m_tMovementDirection)
			m_dwDirectionStartTime	= Level().timeServer();
		else
			if (eMovementDirectionForward != m_tDesirableDirection) {
				m_dwDirectionStartTime	= Level().timeServer();
				m_tDesirableDirection	= eMovementDirectionForward;
			}
			else
				if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tMovementDirection	= eMovementDirectionForward;
				}
	}
	else {
		bool	back = false;
		if (left) {
			if (angle_difference(m_object->head_orientation().current.yaw,yaw) > 2*PI_DIV_3)
				back = true;
		}
		else {
			if (angle_difference(m_object->head_orientation().current.yaw,yaw) > PI_DIV_3)
				back = true;
		}
		if (!back)
			// moving left|right
			if (left) {
//				Msg							("Moving LEFT");
				// moving right, looking left
				if (eMovementDirectionRight == m_tMovementDirection)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (eMovementDirectionRight != m_tDesirableDirection) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionRight;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionRight;
						}
			}
			else {
//				Msg							("Moving RIGHT");
				// moving left, looking right
				if (eMovementDirectionLeft == m_tMovementDirection)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (eMovementDirectionLeft != m_tDesirableDirection) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionLeft;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionLeft;
						}
			}
		else {
//			Msg							("Moving BACK");
			// moving back
			if (eMovementDirectionBack == m_tMovementDirection)
				m_dwDirectionStartTime	= Level().timeServer();
			else
				if (eMovementDirectionBack != m_tDesirableDirection) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tDesirableDirection	= eMovementDirectionBack;
				}
				else
					if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tMovementDirection	= eMovementDirectionBack;
					}
		}
	}
//	Msg("----%d\n[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",Level().timeServer(),yaw,m_object->body_orientation().target.yaw,m_object->body_orientation().current.yaw,m_object->head_orientation().target.yaw,m_object->head_orientation().current.yaw);
//	Msg("Trying %s\nMoving %s",caMovementActionNames[m_tDesirableDirection],caMovementActionNames[m_tMovementDirection]);
//	if (eMentalStateFree == m_object->mental_state())
//		m_tMovementDirection = eMovementDirectionForward;

	MonsterSpace::SBoneRotation		body_orientation = m_object->body_orientation();
	body_orientation.target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_tMovementDirection]);
	m_object->set_body_orientation	(body_orientation);
	
	forward_direction	= false;
	left				= left_angle(-yaw,-m_object->body_orientation().current.yaw);

	if (left) {
		if (angle_difference(yaw,m_object->body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;
	}
	else
		if (angle_difference(yaw,m_object->body_orientation().current.yaw) <= PI_DIV_3)
			forward_direction = true;

	bool				back = false;

	if (left) {
		if (angle_difference(m_object->body_orientation().current.yaw,yaw) > 2*PI_DIV_3)
			back = true;
	}
	else {
		if (angle_difference(m_object->body_orientation().current.yaw,yaw) > PI_DIV_3)
			back = true;
	}
	
	EMovementDirection				direction;
	if (left)
		if (forward_direction)
			direction				= eMovementDirectionForward;
		else
			if (back)
				direction			= eMovementDirectionBack;
			else
				direction			= eMovementDirectionRight;
	else
		if (forward_direction)
			direction				= eMovementDirectionForward;
		else
			if (back)
				direction			= eMovementDirectionBack;
			else
				direction			= eMovementDirectionLeft;

	tpLegsAnimation					= m_tAnims.A[l_tBodyState].m_tMoves.A[m_object->movement_type()].A[direction].A[0];
	m_object->adjust_speed_to_animation(direction);
}

static void	HeadPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*m_object = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(m_object);
	//	m_object->m_tpCurrentHeadAnimation	= 0;
#endif
}

static void	TorsoPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*m_object = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(m_object);
	//	m_object->m_tpCurrentTorsoAnimation	= 0;
#endif
}

static void	LegsPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*m_object = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(m_object);
	m_object->m_tpCurrentLegsAnimation	= 0;
#endif
}

static void	GlobalPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*m_object = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(m_object);
	//	m_object->m_tpCurrentGlobalAnimation	= 0;
#endif
}

static void	ScriptPlayCallback(CBlend *B)
{
	CAI_Stalker							*m_object = (CAI_Stalker*)B->CallbackParam;
#ifdef DEBUG
	VERIFY								(m_object);
//	VERIFY								(!m_object->m_current_script_animation || m_object->m_current_script_animation == m_object->m_script_animations.front().m_motion);
#endif
	if (m_object->m_current_script_animation && !m_object->m_script_animations.empty() && (m_object->m_current_script_animation == m_object->m_script_animations.front().m_motion))
		m_object->m_script_animations.pop_front();
	m_object->m_current_script_animation	= 0;
}

void CAI_Stalker::SelectAnimation(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	CAI_Stalker				*m_object = /**smart_cast<CAI_Stalker*>(/**/this;//);
//	VERIFY					(m_object);
	if (!m_object->g_Alive())
		return;

	CSkeletonAnimated		&tVisualObject		=	*(smart_cast<CSkeletonAnimated*>(m_object->Visual()));

	if (!m_script_animations.empty()) {
		m_tpCurrentGlobalAnimation	= 0;
		m_tpCurrentTorsoAnimation	= 0;
		m_tpCurrentLegsAnimation	= 0;
		if (m_current_script_animation != m_script_animations.front().m_motion)
			tVisualObject.PlayCycle	(m_current_script_animation = m_script_animations.front().m_motion,TRUE,ScriptPlayCallback,this);
		dbg_animation				("Script",m_current_script_animation);
		return;
	}

	if (m_tAnims.A.empty())
		return;

	m_current_script_animation		= 0;
	CMotionDef *tpGlobalAnimation	=	0;
	CMotionDef *tpHeadAnimation		=	0;
	CMotionDef *tpTorsoAnimation	=	0;
	CMotionDef *tpLegsAnimation		=	0;

	vfAssignGlobalAnimation			(tpGlobalAnimation);

	if (tpGlobalAnimation) {
		m_tpCurrentTorsoAnimation	= 0;
		m_tpCurrentLegsAnimation	= 0;
		if (m_tpCurrentGlobalAnimation != tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tVisualObject.PlayCycle(m_tpCurrentGlobalAnimation = tpGlobalAnimation,TRUE,GlobalPlayCallback,this);
			dbg_animation			("Global",tpGlobalAnimation);
		}
	}
	else
		if (m_object->g_Alive()) {
			m_tpCurrentGlobalBlend	= 0;
			m_tpCurrentGlobalAnimation = 0;
			vfAssignTorsoAnimation	(tpTorsoAnimation);
			vfAssignLegsAnimation	(tpLegsAnimation);
			vfAssignHeadAnimation	(tpHeadAnimation);

/*			Msg("stalker %s, visual %s, torso %s, legs %s, head %s", 
					cName(), cNameVisual(),
					tVisualObject.LL_MotionDefName_dbg(tpTorsoAnimation),
					tVisualObject.LL_MotionDefName_dbg(tpLegsAnimation),
					tVisualObject.LL_MotionDefName_dbg(tpHeadAnimation));
*/			

			if (tpHeadAnimation && (m_tpCurrentHeadAnimation != tpHeadAnimation)) {
				m_tpCurrentHeadBlend	= tVisualObject.PlayCycle(m_tpCurrentHeadAnimation = tpHeadAnimation,TRUE,HeadPlayCallback,this);
				dbg_animation			("Head",tpHeadAnimation);
			}
			if (tpTorsoAnimation && (m_tpCurrentTorsoAnimation != tpTorsoAnimation)) {
				m_tpCurrentTorsoBlend	= tVisualObject.PlayCycle(m_tpCurrentTorsoAnimation = tpTorsoAnimation,TRUE,TorsoPlayCallback,this);
				dbg_animation			("Torso",tpTorsoAnimation);
			}
			if (tpLegsAnimation && (m_tpCurrentLegsAnimation != tpLegsAnimation)) {
				m_tpCurrentLegsBlend	= tVisualObject.PlayCycle(m_tpCurrentLegsAnimation = tpLegsAnimation,TRUE,LegsPlayCallback,this);
				dbg_animation			("Legs",tpLegsAnimation);
			}
			
			if (tpTorsoAnimation && tpLegsAnimation){
				if ((tpTorsoAnimation->flags & esmSyncPart) && (tpLegsAnimation->flags & esmSyncPart))
					if (m_tpCurrentTorsoBlend && m_tpCurrentLegsBlend)
						m_tpCurrentTorsoBlend->timeCurrent = m_tpCurrentLegsBlend->timeCurrent;
			}
		}
}

void CStalkerAnimations::add_animation		(LPCSTR animation, bool hand_usage)
{
	CMotionDef						*motion = smart_cast<CSkeletonAnimated*>(m_visual)->ID_Cycle_Safe(animation);
	if (!motion) {
		CObject						*object = smart_cast<CObject*>(this);
		VERIFY						(object);
		ai().script_engine().script_log(eLuaMessageTypeError,"There is no animation %s (object %s)!",animation,*object->cName());
		return;
	}
	m_script_animations.push_back	(CScriptAnimation(hand_usage,motion));
}

void CStalkerAnimations::clear_animations	()
{
	m_script_animations.clear		();
}

void CStalkerAnimations::reinit				()
{
	m_tpCurrentGlobalAnimation		= 
	m_tpCurrentTorsoAnimation		= 
	m_tpCurrentLegsAnimation		= 0;
	m_dwAnimationSwitchInterval		= 500;
	m_dwDirectionStartTime			= 0;
	m_tMovementDirection			= eMovementDirectionForward;
	m_tDesirableDirection			= eMovementDirectionForward;
	m_script_animations.clear		();
}

void CStalkerAnimations::reload				(IRender_Visual *Visual, CInifile *ini, LPCSTR section)
{
	m_object								= smart_cast<CAI_Stalker*>(this);
	VERIFY									(m_object);
	m_visual								= Visual;
	m_tAnims.Load							(smart_cast<CSkeletonAnimated*>(Visual),"");
	m_tHead.Load							(smart_cast<CSkeletonAnimated*>(Visual),"");
	m_tGlobalItem.Load						(smart_cast<CSkeletonAnimated*>(Visual),"item_");
	
	if (!m_object->g_Alive())
		return;

	int										head_bone = smart_cast<CKinematics*>(Visual)->LL_BoneID(ini->r_string(section,"bone_head"));
	smart_cast<CKinematics*>(Visual)->LL_GetBoneInstance	(u16(head_bone)).set_callback(HeadCallback,m_object);

	int										shoulder_bone = smart_cast<CKinematics*>(Visual)->LL_BoneID(ini->r_string(section,"bone_shoulder"));
	smart_cast<CKinematics*>(Visual)->LL_GetBoneInstance	(u16(shoulder_bone)).set_callback(ShoulderCallback,m_object);

	int										spin_bone = smart_cast<CKinematics*>(Visual)->LL_BoneID(ini->r_string(section,"bone_spin"));
	smart_cast<CKinematics*>(Visual)->LL_GetBoneInstance	(u16(spin_bone)).set_callback(SpinCallback,m_object);
};

void CAI_Stalker::adjust_speed_to_animation	(const EMovementDirection movement_direction)
{
	if (body_state() == eBodyStateStand) {
		if (movement_direction != eMovementDirectionBack) {
			if (movement_type() == eMovementTypeWalk)
				set_desirable_speed(m_fCurSpeed = m_fWalkFactor);
			else
				if (movement_type() == eMovementTypeRun)
					set_desirable_speed(m_fCurSpeed = m_fRunFactor);
		}
		else {
			if (movement_type() == eMovementTypeWalk)
				set_desirable_speed(m_fCurSpeed = m_fWalkBackFactor);
			else
				if (movement_type() == eMovementTypeRun)
					set_desirable_speed(m_fCurSpeed = m_fRunBackFactor);
		}
	}
}

void CAI_Stalker::dbg_animation				(LPCSTR caption, CMotionDef *animation)
{
#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation))
		Msg			("%6d [%s][%s][%s]",Level().timeServer(),*cName(),caption,smart_cast<CSkeletonAnimated*>(Visual())->LL_MotionDefName_dbg(animation));
#endif
}
