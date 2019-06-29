////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../ai_monsters_anims.h"

class CMotionDef;
class CBlend;

extern LPCSTR caStateNames			[];
extern LPCSTR caWeaponNames			[];
extern LPCSTR caWeaponActionNames	[];
extern LPCSTR caFoodNames			[];
extern LPCSTR caFoodActionNames		[];
extern LPCSTR caMovementNames		[];
extern LPCSTR caMovementActionNames	[];
extern LPCSTR caInPlaceNames		[];
extern LPCSTR caGlobalNames			[];
extern LPCSTR caHeadNames			[];
extern float  faTurnAngles			[];

class CAI_Stalker;

class CStateAnimations {
public:
	typedef CAniCollection<CAniVector,	caMovementActionNames>	CMovementActions;
	typedef CAniCollection<CAniVector,	caWeaponActionNames>	CWeaponActions;
	
	// global
	CAniCollection<CAniVector,		caGlobalNames>		m_tGlobal;
	// torso
	CAniCollection<CWeaponActions,	caWeaponNames>		m_tTorso;
	// legs
	CAniCollection<CMovementActions,caMovementNames>	m_tMoves;
	// in-place 
	CAniFVector	  <caInPlaceNames>						*m_tInPlace;
					CStateAnimations	();
					CStateAnimations	(const CStateAnimations &anims);
	virtual			~CStateAnimations	();
			void	Load				(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName);
};

class CStalkerAnimations {
public:
	struct CScriptAnimation {
		bool		m_hand_usage;
		CMotionDef	*m_motion;

		IC			CScriptAnimation	(bool hand_usage, CMotionDef *motion) :
						m_hand_usage(hand_usage),
						m_motion	(motion)
		{
		}
	};

	CAniCollection<CStateAnimations,caStateNames>	m_tAnims;
	// head
	CAniFVector<caHeadNames>						m_tHead;
	// weapon item
	CAniCollection<CStateAnimations::CWeaponActions,caWeaponNames>		m_tGlobalItem;

	CMotionDef					*m_tpCurrentGlobalAnimation;
	CMotionDef					*m_tpCurrentHeadAnimation;
	CMotionDef					*m_tpCurrentTorsoAnimation;
	CMotionDef					*m_tpCurrentLegsAnimation;
	CMotionDef					*m_current_script_animation;
	CBlend						*m_tpCurrentGlobalBlend;
	CBlend						*m_tpCurrentHeadBlend;
	CBlend						*m_tpCurrentTorsoBlend;
	CBlend						*m_tpCurrentLegsBlend;
	u8							m_bAnimationIndex;
	u32							m_dwDirectionStartTime;
	u32							m_dwAnimationSwitchInterval;
	EMovementDirection			m_tMovementDirection;
	EMovementDirection			m_tDesirableDirection;
	xr_deque<CScriptAnimation>	m_script_animations;
	IRender_Visual				*m_visual;
	CAI_Stalker					*m_object;
	
	virtual	void				reinit							();
	virtual	void				reload							(IRender_Visual *Visual, CInifile *ini, LPCSTR section);
	static	void __stdcall		HeadCallback					(CBoneInstance *B);
	static	void __stdcall		ShoulderCallback				(CBoneInstance *B);
	static	void __stdcall		SpinCallback					(CBoneInstance *B);
			void				vfAssignGlobalAnimation			(CMotionDef *&tpGlobalAnimation);
			void				vfAssignHeadAnimation			(CMotionDef *&tpGlobalAnimation);
			void				vfAssignTorsoAnimation			(CMotionDef *&tpGlobalAnimation);
			void				vfAssignLegsAnimation			(CMotionDef *&tpLegsAnimation);
			void				add_animation					(LPCSTR animation, bool hand_usage = false);
			void				clear_animations				();
};