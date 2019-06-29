////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.h
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_selector_template.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\actor.h"

using namespace AI;

class CAI_Soldier : public CCustomMonster
{

	typedef	CCustomMonster inherited;

	enum ESoundCcount {
		SND_HIT_COUNT=4,
		SND_DIE_COUNT=4,
		SND_RADIO_COUNT=4,
		SND_STEP_COUNT=2,
	};

	enum ESoldierStates 	{
		aiSoldierDie = 0,
		aiSoldierTurnOver,
		aiSoldierWaitForAnimation,
		aiSoldierWaitForTime,
		aiSoldierRecharge,
		aiSoldierLookingOver,
		
		aiSoldierPatrol,
		aiSoldierPatrolReturn,
		aiSoldierPatrolRoute,
		aiSoldierPatrolWait,
		
		aiSoldierFight,
		aiSoldierFightAlone,
		aiSoldierFightGroup,
		aiSoldierAttackAlone,
		aiSoldierDefendAlone,
		aiSoldierPursuitAlone,
		aiSoldierFindAlone,
		aiSoldierRetreatAlone,
		aiSoldierHurtAlone,
		aiSoldierAttackAloneNonFire,
		aiSoldierAttackAloneFire,
		aiSoldierDefendAloneNonFire,
		aiSoldierDefendAloneFire,
		aiSoldierPursuitAloneNonFire,
		aiSoldierPursuitAloneFire,
		aiSoldierFindAloneNonFire,
		aiSoldierFindAloneFire,
		aiSoldierRetreatAloneNonFire,
		aiSoldierRetreatAloneFire,
		aiSoldierRetreatAloneDialog,
		aiSoldierAttackAloneNonFireFire,
		aiSoldierAttackAloneNonFireRun,
		aiSoldierAttackAloneNonFireSteal,
		aiSoldierAttackAloneNonFireDialog,
		aiSoldierAttackAloneFireFire,
		aiSoldierAttackAloneFireRun,
		aiSoldierAttackAloneFireSteal,
		aiSoldierAttackAloneFireDialog,
		aiSoldierDefendAloneNonFireFire,
		aiSoldierDefendAloneNonFireRun,
		aiSoldierDefendAloneNonFireSteal,
		aiSoldierDefendAloneNonFireDialog,
		aiSoldierDefendAloneFireFire,
		aiSoldierDefendAloneFireRun,
		aiSoldierDefendAloneFireSteal,
		aiSoldierDefendAloneFireDialog,
		aiSoldierAttackGroup,
		aiSoldierDefendGroup,
		aiSoldierPursuitGroup,
		aiSoldierFindGroup,
		aiSoldierRetreatGroup,
		aiSoldierHurtGroup,
		aiSoldierAttackGroupNonFire,
		aiSoldierAttackGroupFire,
		aiSoldierDefendGroupNonFire,
		aiSoldierDefendGroupFire,
		aiSoldierPursuitGroupNonFire,
		aiSoldierPursuitGroupFire,
		aiSoldierFindGroupNonFire,
		aiSoldierFindGroupFire,
		aiSoldierRetreatGroupNonFire,
		aiSoldierRetreatGroupFire,
		aiSoldierRetreatGroupDialog,
		aiSoldierAttackGroupNonFireFire,
		aiSoldierAttackGroupNonFireRun,
		aiSoldierAttackGroupNonFireSteal,
		aiSoldierAttackGroupNonFireDialog,
		aiSoldierAttackGroupFireFire,
		aiSoldierAttackGroupFireRun,
		aiSoldierAttackGroupFireSteal,
		aiSoldierAttackGroupFireDialog,
		aiSoldierDefendGroupNonFireFire,
		aiSoldierDefendGroupNonFireRun,
		aiSoldierDefendGroupNonFireSteal,
		aiSoldierDefendGroupNonFireDialog,
		aiSoldierDefendGroupFireFire,
		aiSoldierDefendGroupFireRun,
		aiSoldierDefendGroupFireSteal,
		aiSoldierDefendGroupFireDialog,
	};
	
	enum EGestureStates {
		GESTURE_STATE_NONE=0,
		GESTURE_STATE_POINT,
		GESTURE_STATE_RAISE_HAND,
		GESTURE_STATE_GO_AHEAD,
	};

	enum EMovementTypes {
		WALK_FORWARD_0=char(0),
		WALK_FORWARD_1,
		WALK_FORWARD_2,
		WALK_FORWARD_3,
		WALK_FORWARD_4,
		RUN_FORWARD_0,
		RUN_FORWARD_1,
		RUN_FORWARD_2,
		RUN_FORWARD_3,
		WALK_BACK_0,
		WALK_BACK_1,
		WALK_BACK_2,
		WALK_LEFT,
		WALK_RIGHT,
		WALK_NO,
	};

	enum EFightTypes {
		FIGHT_TYPE_ATTACK = 0,
		FIGHT_TYPE_DEFEND,
		FIGHT_TYPE_RETREAT,
		FIGHT_TYPE_PURSUIT,
		FIGHT_TYPE_FIND,
		FIGHT_TYPE_HURT,
		FIGHT_TYPE_NONE,
	};

	enum EActionTypes {
		ACTION_TYPE_GROUP = 0,
		ACTION_TYPE_ALONE,
		ACTION_TYPE_DIE,
		ACTION_TYPE_NONE,
	};

	#define MAX_STATE_LIST_SIZE 256
	#define MAX_DYNAMIC_OBJECTS 32
	#define MAX_DYNAMIC_SOUNDS  32
	#define MAX_HURT_COUNT		32
	#define MAX_SEARCH_COUNT	32

	#define	MAX_HEAD_TURN_ANGLE				(2.f*PI_DIV_6)
	typedef struct tagSSoldierStates {
		ESoldierStates	eState;
		u32			dwTime;
	}SSoldierStates;

	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	}SHurt;

	typedef struct tagSSearch {
		u32	dwTime;
		u32	dwNodeID;
	} SSearch;

	u32 m_tActionType;
	u32 m_tFightType;

//	typedef struct tagSSearchPoint {
//		BYTE		cIndex;
//		Fvector		tPoint;
//	} SSearchPoint;

	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousPoints;
	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousForces;

	protected:
		
		svector<SDynamicObject,	MAX_DYNAMIC_OBJECTS>	tpaDynamicObjects;
		svector<SDynamicSound,	MAX_DYNAMIC_SOUNDS>		tpaDynamicSounds;
		svector<SHurt,			MAX_HURT_COUNT>			tpaHurts;
		svector<SSearch,		MAX_SEARCH_COUNT>		tpaSearchPositions;
		u32					m_dwMaxDynamicObjectsCount;
		u32					m_dwMaxDynamicSoundsCount;
		float					m_fSensetivity;
		int						m_iSoundIndex;

		////////////////////////////////////////////////////////////////////////////
		// normal animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSNormalGlobalAnimations{
			CMotionDef* tpaDeath[5];
			CMotionDef* tpaRunForward[4];
			CMotionDef* tpaWalkForward[5];
			CMotionDef* tpaWalkBack[3];
			CMotionDef* tpaLieDown[3];
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpPointSign; // !
			CMotionDef* tpaIdle[2];
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpAim;
			CMotionDef* tpAttack;
			CMotionDef* tpReload;
		}SNormalGlobalAnimations;

		// torso animations
		typedef struct tagSNormalTorsoAnimations{
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
		}SNormalTorsoAnimations;

		// legs animations
		typedef struct tagSNormalLegsAnimations{
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SNormalLegsAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations  tTorso;
			SNormalLegsAnimations	tLegs;
		}SNormalAnimations;

		////////////////////////////////////////////////////////////////////////////
		// crouch animations xyu
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSCrouchGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpIdle;
			CMotionDef* tpaLieDown[2];
			CMotionDef* tpaWalkForward[2];
			CMotionDef* tpaWalkBack[2];
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpPointSign;
			CMotionDef* tpAim;
			CMotionDef* tpAttack;
			CMotionDef* tpReload;
		}SCrouchGlobalAnimations;

		// torso animations
		typedef struct tagSCrouchTorsoAnimations{
			CMotionDef* tpDamageLeft;
			CMotionDef* tpDamageRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
		}SCrouchTorsoAnimations;

		// legs animations
		typedef struct tagSCrouchLegsAnimations{
			CMotionDef* tpJumpBegin;
			CMotionDef* tpJumpIdle;
		}SCrouchLegsAnimations;

		// crouch animations
		typedef struct tagSCrouchAnimations{
			SCrouchGlobalAnimations	tGlobal;
			SCrouchTorsoAnimations  tTorso;
			SCrouchLegsAnimations	tLegs;
		}SCrouchAnimations;

		////////////////////////////////////////////////////////////////////////////
		// lie animations
		////////////////////////////////////////////////////////////////////////////

		// global animations
		typedef struct tagSLieGlobalAnimations{
			CMotionDef* tpDeath;
			CMotionDef* tpStandUp;
			CMotionDef* tpIdle;
			CMotionDef* tpReload;
			CMotionDef* tpAttack;
			CMotionDef* tpDamage;
			CMotionDef* tpTurnLeft;
			CMotionDef* tpTurnRight;
			CMotionDef* tpWalkForward;
			CMotionDef* tpWalkBack;
			CMotionDef* tpWalkLeft;
			CMotionDef* tpWalkRight;
			CMotionDef* tpRaiseHandSign;
			CMotionDef* tpGoAheadSign;
			CMotionDef* tpPointSign;
		}SLieGlobalAnimations;

		// lie animations
		typedef struct tagSLieAnimations{
			SLieGlobalAnimations	tGlobal;
		} SLieAnimations;

		////////////////////////////////////////////////////////////////////////////
		// soldier animations
		////////////////////////////////////////////////////////////////////////////

		typedef struct tagSSoldierAnimations{
			SNormalAnimations	tNormal;
			SCrouchAnimations	tCrouch;
			SLieAnimations		tLie;
		} SSoldierAnimations;

		SSoldierAnimations		tSoldierAnimations;
		CMotionDef*				m_tpCurrentGlobalAnimation;
		CMotionDef*				m_tpCurrentTorsoAnimation;
		CMotionDef*				m_tpCurrentLegsAnimation;
		CMotionDef*				m_tpAnimationBeingWaited;
		CBlend*					m_tpCurrentGlobalBlend;
		CBlend*					m_tpCurrentTorsoBlend;
		CBlend*					m_tpCurrentLegsBlend;
		char					m_cGestureState;
		//float					m_fAddAngle;
		u32						m_dwTimeBeingWaited;
		EMovementTypes			m_cMovementType;
		CMotionDef*				m_tpaMovementAnimations[3][WALK_NO];
		ref_sound*					m_tpSoundBeingPlayed;
		u32						m_dwLastSoundRefresh;
		float					m_fMinRadioIinterval;
		float					m_fMaxRadioIinterval;
		float					m_fRadioRefreshRate;
		u32						m_dwLastRadioTalk;
		float					m_fDistanceWent;
		char					m_cStep;

		// head turns
		static void __stdcall HeadSpinCallback(CBoneInstance*);
		static void	__stdcall LegsSpinCallback(CBoneInstance*);
		
		// mea
		ref_sound					sndHit[SND_HIT_COUNT];
		ref_sound					sndDie[SND_DIE_COUNT];
		ref_sound					sndRadio[SND_RADIO_COUNT];
		ref_sound					sndSteps[SND_STEP_COUNT];
		
		// events
		EVENT					m_tpEventSay;
		EVENT					m_tpEventAssignPath;
		
		// ai
		ESoldierStates			m_eCurrentState;
		ESoldierStates			m_ePreviousState;
		bool					m_bStopThinking;
		u32						m_dwLastUpdate;
		u32						m_dwCurrentUpdate;
		u32						m_dwUpdateCount;
//		float			m_fAddAngle;
		
		// action data
		bool					m_bJumping;
		
		// hit data
		u32						m_dwHitTime;
		Fvector					m_tHitDir;
		Fvector					m_tHitPosition;
		
		// visual data
		xr_vector<CObject*>		tpaVisibleObjects;
		
		// saved enemy
		SEnemySelected			Enemy;
		CEntity*				tSavedEnemy;
		Fvector					tSavedEnemyPosition;
		u32						dwLostEnemyTime;
		NodeCompressed*			tpSavedEnemyNode;
		u32						dwSavedEnemyNodeID;
		bool					bBuildPathToLostEnemy;
		int						m_iCurrentSuspiciousNodeIndex;
		SuspiciousPoints		m_tpaSuspiciousPoints;
		SuspiciousForces		m_tpaSuspiciousForces;
		xr_vector<u32>			m_tpaNodeStack;

		
		// performance data
		u32						m_dwLastRangeSearch;
		u32						m_dwLastSuccessfullSearch;
		
		// characteristics
		float					m_fAggressiveness;
		float					m_fTimorousness;
		
		// visibility constants
		u32						m_dwMovementIdleTime;
		float					m_fMaxInvisibleSpeed;
		float					m_fMaxViewableSpeed;
		float					m_fMovementSpeedWeight;
		float					m_fDistanceWeight;
		float					m_fSpeedWeight;
		float					m_fCrouchVisibilityMultiplier;
		float					m_fLieVisibilityMultiplier;
		float					m_fVisibilityThreshold;
		float					m_fLateralMultiplier;
		float					m_fShadowWeight;
		
		// firing
		bool					m_bFiring;
		u32						m_dwStartFireAmmo;
		u32						m_dwNoFireTime;
		float					m_fAddWeaponAngle;
		
		// fire  constants
		u32						m_dwFireRandomMin;
		u32						m_dwFireRandomMax;
		u32						m_dwNoFireTimeMin;
		u32						m_dwNoFireTimeMax;
		float					m_fMinMissDistance;
		float					m_fMinMissFactor;
		float					m_fMaxMissDistance;
		float					m_fMaxMissFactor;

		// patrol structures
		bool					m_bLessCoverLook;
		float					m_fMinPatrolDistance;
		float					m_fMaxPatrolDistance;

		// finite state machine
		xr_stack<ESoldierStates>	m_tStateStack;
		svector<SSoldierStates,MAX_STATE_LIST_SIZE>	m_tStateList;
		bool					m_bStateChanged;
		
		
//		CAISelectorTemplate<
//			aiSearchRange | 
//			aiLightWeight | 
//			aiTotalCover | 
//			aiEnemyDistance | 
//			aiCoverFromEnemyWeight | 
//			aiEnemyViewDeviationWeight | 
//			aiMemberDanger | 
//			aiMemberDistance | 
//			aiEnemySurround>		
		CAI_NodeEvaluatorFull				SelectorAttack;
		CAI_NodeEvaluatorFull				SelectorDefend;
		CAI_NodeEvaluatorFull				SelectorFindEnemy;
		CAI_NodeEvaluatorFull				SelectorFollowLeader;
		CAI_NodeEvaluatorFull				SelectorFreeHunting;
		CAI_NodeEvaluatorFull				SelectorMoreDeadThanAlive;
		CAI_NodeEvaluatorFull				SelectorNoWeapon;
		CAI_NodeEvaluatorFull				SelectorPatrol;
		CAI_NodeEvaluatorFull				SelectorPursuit;
		CAI_NodeEvaluatorFull				SelectorReload;
		CAI_NodeEvaluatorFull				SelectorRetreat;
		CAI_NodeEvaluatorFull				SelectorSenseSomething;
		CAI_NodeEvaluatorFull				SelectorUnderFireCover;
		CAI_NodeEvaluatorFull				SelectorUnderFireLine;

		void OnDeath();
		void OnTurnOver();
		void OnWaitForAnimation();
		void OnWaitForTime();
		void OnRecharge();
		void OnLookingOver();
		
		void OnPatrol();
		void OnPatrolReturn();
		void OnPatrolRoute();
		void OnPatrolWait();

		void OnFight();
		void OnFightAlone();
		void OnFightGroup();
		void OnAttackAlone();
		void OnDefendAlone();
		void OnPursuitAlone();
		void OnFindAlone();
		void OnRetreatAlone();
		void OnHurtAlone();
		void OnAttackAloneNonFire();
		void OnAttackAloneFire();
		void OnDefendAloneNonFire();
		void OnDefendAloneFire();
		void OnPursuitAloneNonFire();
		void OnPursuitAloneFire();
		void OnFindAloneNonFire();
		void OnFindAloneFire();
		void OnRetreatAloneNonFire();
		void OnRetreatAloneFire();
		void OnRetreatAloneDialog();
		void OnAttackAloneNonFireFire();
		void OnAttackAloneNonFireRun();
		void OnAttackAloneNonFireSteal();
		void OnAttackAloneNonFireDialog();
		void OnAttackAloneFireFire();
		void OnAttackAloneFireRun();
		void OnAttackAloneFireSteal();
		void OnAttackAloneFireDialog();
		void OnDefendAloneNonFireFire();
		void OnDefendAloneNonFireRun();
		void OnDefendAloneNonFireSteal();
		void OnDefendAloneNonFireDialog();
		void OnDefendAloneFireFire();
		void OnDefendAloneFireRun();
		void OnDefendAloneFireSteal();
		void OnDefendAloneFireDialog();
		void OnAttackGroup();
		void OnDefendGroup();
		void OnPursuitGroup();
		void OnFindGroup();
		void OnRetreatGroup();
		void OnHurtGroup();
		void OnAttackGroupNonFire();
		void OnAttackGroupFire();
		void OnDefendGroupNonFire();
		void OnDefendGroupFire();
		void OnPursuitGroupNonFire();
		void OnPursuitGroupFire();
		void OnFindGroupNonFire();
		void OnFindGroupFire();
		void OnRetreatGroupNonFire();
		void OnRetreatGroupFire();
		void OnRetreatGroupDialog();
		void OnAttackGroupNonFireFire();
		void OnAttackGroupNonFireRun();
		void OnAttackGroupNonFireSteal();
		void OnAttackGroupNonFireDialog();
		void OnAttackGroupFireFire();
		void OnAttackGroupFireRun();
		void OnAttackGroupFireSteal();
		void OnAttackGroupFireDialog();
		void OnDefendGroupNonFireFire();
		void OnDefendGroupNonFireRun();
		void OnDefendGroupNonFireSteal();
		void OnDefendGroupNonFireDialog();
		void OnDefendGroupFireFire();
		void OnDefendGroupFireRun();
		void OnDefendGroupFireSteal();
		void OnDefendGroupFireDialog();

		// miscellanious funtions	
		bool bfAddEnemyToDynamicObjects(CAI_Soldier *tpSoldier);
		float ffGetCoverFromNode(CAI_Space &AI, Fvector &tPosition, NodeCompressed *tpNode, float fEyeFov);
		bool bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRyPick = false);
		int  ifGetSuspiciousAvailableNode(int iLastIndex, CGroup &Group);
		void vfClasterizeSuspiciousNodes(CGroup &Group);
		void vfFindAllSuspiciousNodes(u32 StartNode, Fvector tPointPosition, const Fvector& BasePos, float Range, CGroup &Group);
		
		int	 ifFindDynamicObject(CEntity *tpEntity);
		bool bfSaveFromEnemy(CEntity *tpEntity);
		bool bfSetLookToDangerPlace();
		bool bfCheckForDangerPlace();
		u32 tfGetAloneFightType();
		u32 tfGetGroupFightType();
		u32 tfGetActionType();
		bool bfCheckForDanger();
		void vfStopFire();
		void SelectSound(int &iIndex);
		void vfUpdateSounds(u32 dwTimeDelta);
		bool bfCheckForEntityVisibility(CEntity *tpEntity);
		bool bfCheckForVisibility(CEntity* tpEntity);
		bool bfCheckForVisibility(int iTestNode, SRotation tMyRotation, bool bRotation);
		void vfLoadSounds();
		void vfLoadSelectors(LPCSTR section);
		void vfAssignBones(CInifile *ini, const char *section);
		void vfLoadAnimations();
		bool bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
		bool bfCheckPath(AI::NodePath &Path);
		void SetLessCoverLook(NodeCompressed *tNode, bool bSpine = true);
		void SetLessCoverLook();
		void SetDirectionLook();
		void SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection);
		void vfInitSelector(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
		void vfBuildPathToDestinationPoint(IBaseAI_NodeEvaluator *S);
		void vfSearchForBetterPosition(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
		void vfSearchForBetterPositionWTime(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
		void vfAimAtEnemy(bool bInaccuracy = false);
		void vfSaveEnemy();
		
		void vfMarkVisibleNodes(CEntity *tpEntity);
		float ffGetDistanceToNearestMember();
		bool bfCheckIfIHaveToChangePosition();
		bool bfCheckIfCanKillMember();
		bool bfCheckIfCanKillEnemy();
		void vfSetFire(bool bFire, CGroup &Group);
		void vfCheckForSavedEnemy();
		void vfUpdateDynamicObjects();
		void SetLook(Fvector tPosition);
		void vfSetMovementType(EMovementTypes cMovementType, float fMultiplier = 1.0f);
	IC	void vfSetLookAndFireMovement(bool a, EMovementTypes c, float d, CGroup &Group, u32 dwCurTime)
		{
			if (!(Group.m_bLessCoverLook)) {
				Group.m_bLessCoverLook = m_bLessCoverLook = true;
				Group.m_dwLastViewChange = dwCurTime;
			}
			else
				if ((m_bLessCoverLook) && (dwCurTime - Group.m_dwLastViewChange > 5000))
					Group.m_bLessCoverLook = m_bLessCoverLook = false;
			if (m_bLessCoverLook) {
				if (!bfSetLookToDangerPlace())
					SetLessCoverLook();
			}
			else
				SetDirectionLook();
			vfSetFire(a,Group);
			vfSetMovementType(c,d);
		}
	IC	bool bfAmIDead()		{return(g_Health() <= 0);}
	IC	bool bfAmIHurt()		{return(m_dwCurrentUpdate >= m_dwHitTime);}
	IC	bool bfIsMemberHurt()	{return(m_dwCurrentUpdate >= getGroup()->m_dwLastHitTime);}
	IC  bool bfDoesEnemyExist()	{return(Enemy.Enemy != 0);}
	IC  bool bfIsEnemyVisible()	{return(Enemy.bVisible);}
	IC  bool bfFireEnemy(CEntity *tpEntity)		
		{
			VERIFY(tpEntity);
			CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
			if (tpCustomMonster)
				if (tpCustomMonster->GetItemList())
					return(true);
				else
					return(false);
			else {
				CActor *tpActor = dynamic_cast<CActor *>(tpEntity);
				if (tpActor)
					return(true);
				else
					return(false);
			}
		}
	IC	bool bfTooBigDistance(Fvector tPosition, float fDistance)
		{
			return(tPosition.distance_to(Position()) > fDistance);
		}
	IC	bool bfTooFarToEnemy(CEntity *tpEntity, float fDistance)
		{
			return(bfTooBigDistance(tpEntity->Position(),fDistance));
		}
	IC	bool bfNeedRecharge()
		{
			//return(Weapons && (Weapons->getAmmoElapsed() == 0));
			return(false);
		}
	IC	bool bfNoAmmo()
		{
			return(true);
		}
	IC	u32 tfUpdateActionType()
		{
			return(m_tActionType = tfGetActionType());
		}
	IC	u32 tfUpdateFightTypeAlone()
		{
			return(m_tFightType = tfGetAloneFightType());
		}
	IC	u32 tfUpdateFightTypeGroup()
		{
			return(m_tFightType = tfGetGroupFightType());
		}
	IC	bool bfCheckIfActionTypeChanged()
		{
			return(tfGetActionType() != m_tActionType);
		}
	IC	bool bfCheckIfFightTypeChanged()
		{
			return(((m_tActionType == ACTION_TYPE_ALONE) && (tfGetAloneFightType() != m_tFightType)) || ((m_tActionType == ACTION_TYPE_GROUP) && (tfGetGroupFightType() != m_tFightType)) || ((m_tActionType != ACTION_TYPE_ALONE) && (m_tActionType != ACTION_TYPE_GROUP)));
		}
	IC	bool bfCheckIfActionOrFightTypeChanged()
		{
			return(bfCheckIfActionTypeChanged() || bfCheckIfFightTypeChanged());
		}
	IC	void vfAddStateToList(ESoldierStates eState)
		{
			if ((m_tStateList.size()) && (m_tStateList[m_tStateList.size() - 1].eState == eState)) {
				m_tStateList[m_tStateList.size() - 1].dwTime = m_dwCurrentUpdate;
				return;
			}
			if (m_tStateList.size() >= MAX_STATE_LIST_SIZE)
				m_tStateList.erase(u32(0));
			SSoldierStates tSoldierStates;
			tSoldierStates.dwTime = m_dwCurrentUpdate;
			tSoldierStates.eState = eState;
			m_tStateList.push_back(tSoldierStates);
		}
	IC	void vfAddHurtToList(CEntity *tpEntity)
		{
			if ((tpaHurts.size()) && (tpaHurts[tpaHurts.size() - 1].tpEntity == tpEntity)) {
				tpaHurts[tpaHurts.size() - 1].dwTime = m_dwCurrentUpdate;
				return;
			}
			if (tpaHurts.size() >= MAX_STATE_LIST_SIZE)
				tpaHurts.erase(u32(0));
			SHurt tHurt;
			tHurt.dwTime = m_dwCurrentUpdate;
			tHurt.tpEntity = tpEntity;
			tpaHurts.push_back(tHurt);
		}
	IC	bool bfCheckHistoryForState(ESoldierStates eState, u32 dwTimeInterval)
		{
			for (int i=m_tStateList.size() - 2; i>=0; i--)
				if (m_tStateList[i].eState == eState)
					return(true);
				else
					if (m_dwCurrentUpdate - m_tStateList[i].dwTime > dwTimeInterval)
						return(false);
			return(false);
		}
	IC	void vfAddToSearchList()
		{
			if (tpaSearchPositions.size() >= MAX_SEARCH_COUNT)
				tpaSearchPositions.erase(u32(0));
			SSearch tSearch;
			tSearch.dwTime = m_dwCurrentUpdate;
			tSearch.dwNodeID = AI_Path.DestNode;
			tpaSearchPositions.push_back(tSearch);
		}

	public:
		bool		  m_bActionStarted;
					   CAI_Soldier();
		virtual		  ~CAI_Soldier();
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element);
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual void  Die();
		virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
//		virtual void  g_fireParams(Fvector &fire_pos, Fvector &fire_dir);
		virtual void  shedule_Update(u32 DT);
		virtual void  renderable_Render(); 
		virtual void  Exec_Movement(float dt);
		virtual void  OnEvent(EVENT E, u64 P1, u64 P2);
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual	void  feel_sound_new(CObject* who, int eType, Fvector& Position, float power);
		virtual BOOL  feel_vision_isRelevant(CObject* who);
};