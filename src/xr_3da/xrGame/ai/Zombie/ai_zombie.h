////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie.h
//	Created 	: 23.04.2002
//  Modified 	: 26.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_ZOMBIE__
#define __XRAY_AI_ZOMBIE__

#include "../../CustomMonster.h"
#include "../../level_graph.h"

class CAI_Zombie : public CCustomMonster  
{
protected:
		//////////////////////////
		// STRUCTURES
		//////////////////////////
		#define TIME_TO_GO		2000
		#define TIME_TO_RETURN	 500

		enum ESoundCcount {
			SND_ATTACK_COUNT=4,
			SND_DEATH_COUNT=3,
			SND_HIT_COUNT=4,
			SND_IDLE_COUNT=3,
			SND_NOTICE_COUNT=1,
			SND_PURSUIT_COUNT=6,
			SND_RESURRECT_COUNT=6,
		};

		enum EZombieStates 	{
			aiZombieDie = 0,
			aiZombieTurn,
			aiZombieFreeHuntingActive,
			aiZombieFreeHuntingPassive,
			aiZombieAttackFire,
			aiZombieAttackRun,
			aiZombiePursuit,
			aiZombieReturnHome,
			aiZombieResurrect,
		};

	
		typedef	CCustomMonster inherited;

		typedef struct tagSNormalGlobalAnimations{
			CMotionDef *tpaDeath[3];
			CMotionDef *tpaDeathIdle[3];
			CMotionDef *tpaStandUp[3];
			CMotionDef *tpaAttack[3];
			CMotionDef *tpaIdle[2];
			SAnimState  tWalk;
			CMotionDef *tpWalkForwardCSIP;
			CMotionDef *tpRunForward;
			CMotionDef *tpTurnLeft;
			CMotionDef *tpTurnRight;
		}SNormalGlobalAnimations;

		typedef struct tagSNormalTorsoAnimations{
			CMotionDef *tpDamageLeft;
			CMotionDef *tpDamageRight;
///			CMotionDef *tpBlaBlaBla0;
		}SNormalTorsoAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
			SNormalTorsoAnimations tTorso;
		}SNormalAnimations;

		typedef struct tagSZombieAnimations{
			SNormalAnimations	tNormal;
		}SZombieAnimations;
		//////////////////////////
		// END OF STRUCTURES
		//////////////////////////

		//////////////////////////
		// CLASS MEMBERS
		//////////////////////////
		
		// FSM
		enum EZombieAction {
			eZombieActionNone = u32(0),
			eZombieActionAttackBegin,
			eZombieActionAttackEnd,
		};

		EZombieAction			m_tAction;
		float					m_fLookSpeed;

		xr_stack<EZombieStates>	m_tStateStack;
		EZombieStates			m_eCurrentState;
		EZombieStates			m_ePreviousState;
		bool					m_bStopThinking;
		bool					m_bStateChanged;

		// ANIMATIONS
		SZombieAnimations	m_tZombieAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		
		// SOUNDS
		ref_sound				m_tpaSoundAttack	[SND_ATTACK_COUNT];
		ref_sound				m_tpaSoundDeath		[SND_DEATH_COUNT];
		ref_sound				m_tpaSoundHit		[SND_HIT_COUNT];
		ref_sound				m_tpaSoundIdle		[SND_IDLE_COUNT];
		ref_sound				m_tpaSoundNotice	[SND_NOTICE_COUNT];
		ref_sound				m_tpaSoundPursuit	[SND_PURSUIT_COUNT];
		ref_sound				m_tpaSoundResurrect	[SND_RESURRECT_COUNT];
		ref_sound*				m_tpSoundBeingPlayed;
		u32					m_dwLastSoundRefresh;
		
		float				m_fMinVoiceIinterval;
		float				m_fMaxVoiceIinterval;
		float				m_fVoiceRefreshRate;
		u32					m_dwLastVoiceTalk;

		float				m_fMinPursuitIinterval;
		float				m_fMaxPursuitIinterval;
		float				m_fPursuitRefreshRate;
		u32					m_dwLastPursuitTalk;

		
		// ATTACK
		bool				m_bActionStarted;
		bool				m_bFiring;
		u32					m_dwStartAttackTime;
		float				m_fAttackSpeed;
		// HIT
		u32					m_hit_time;
		Fvector				m_hit_direction;
		Fvector				m_tHitPosition;
		float				m_fHitPower;
		u32					m_dwHitInterval;
		
		typedef struct tagSSimpleSound {
			ESoundTypes		eSoundType;
			u32				dwTime;
			float			fPower;
			Fvector			tSavedPosition;
			CEntityAlive	*tpEntity;
		} SSimpleSound;

		// SOUND BEING FELT
		SSimpleSound		m_tLastSound;
		
		// ENEMY
		CEntityAlive*		m_tSavedEnemy;
		Fvector				m_tSavedEnemyPosition;
		u32					m_dwLostEnemyTime;
		const CLevelGraph::CVertex* 	m_tpSavedEnemyNode;
		u32					m_dwSavedEnemyNodeID;
		
		// PERFORMANCE
		u32					m_previous_query_time;
		
		// BEHAVIOUR
		Fvector				m_tGoalDir;
		Fvector				m_tCurrentDir;
		Fvector				m_tHPB;
		float				m_fDHeading;

		// constants
		float				m_fGoalChangeDelta;
		float				m_fSpeed;
		float				m_fSafeSpeed;
		float				m_fASpeed;
		Fvector				m_tVarGoal;
		float				m_fIdleSoundDelta;
		Fvector				m_tSpawnPosition;
		Fvector				m_tSafeSpawnPosition;
		float				m_fAngleSpeed;
		float				m_fSafeGoalChangeDelta;
		Fvector				m_tGoalVariation;

		// variables
		float				m_fGoalChangeTime;
		Fvector				m_tOldPosition;
		bool				m_bNoWay;

		// active
		float				m_fChangeActiveStateProbability;
		u32					m_dwActiveCountPercent;
		u32					m_dwActiveScheduleMin;
		u32					m_dwActiveScheduleMax;
		u32					m_dwPassiveScheduleMin;
		u32					m_dwPassiveScheduleMax;
		bool				m_bActive;

		// attack parameters
		float				m_fAttackDistance;
		float				m_fAttackAngle;
		float				m_fMaxPursuitRadius;
		float				m_fMaxHomeRadius;

		// former constants
		u32					m_dwLostMemoryTime;
		float				m_fAttackStraightDistance;
		float				m_fStableDistance;
		float				m_fWallMinTurnValue;
		float				m_fWallMaxTurnValue;
		float				m_fSoundThreshold;
		u32					m_dwTimeToLie;
		u32					m_dwToWaitBeforeDestroy;

		float				m_fMaxSpeed;
		float				m_fMinSpeed;

		//////////////////////////
		// INLINE FUNCTIONS
		//////////////////////////
		IC void vfChangeGoal()
		{
			Fvector vP;
			vP.set(m_tSpawnPosition.x,m_tSpawnPosition.y,m_tSpawnPosition.z);
			m_tGoalDir.x = vP.x+m_tVarGoal.x*::Random.randF(-0.5f,0.5f); 
			m_tGoalDir.y = vP.y+m_tVarGoal.y*::Random.randF(-0.5f,0.5f);
			m_tGoalDir.z = vP.z+m_tVarGoal.z*::Random.randF(-0.5f,0.5f);
		}
		
		IC bool bfCheckIfGoalChanged(bool bForceChangeGoal = true)
		{
			if (m_fGoalChangeTime<=0){
				m_fGoalChangeTime += m_fGoalChangeDelta+m_fGoalChangeDelta*::Random.randF(-0.5f,0.5f);
				if (bForceChangeGoal)
					vfChangeGoal();
				return(true);
			}
			return(false);
		};

		IC void vfChooseNewSpeed()
		{
			int iRandom = ::Random.randI(0,2);
			switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
				}
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
				}
			}
			m_fSafeSpeed = m_fSpeed;
		};

		IC void vfUpdateTime(float fTimeDelta)
		{
			m_fGoalChangeTime -= fTimeDelta > .1f ? .1f : fTimeDelta;
		};		

		IC void vfAddActiveMember(bool bForceActive = false)
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if (!m_bActive && (bForceActive || (Group.m_dwAliveCount*m_dwActiveCountPercent/100 >= Group.m_dwActiveCount))) {
				m_bActive = true;
				m_eCurrentState = aiZombieFreeHuntingActive;
				++Group.m_dwActiveCount;
				shedule.t_min	= m_dwActiveScheduleMin;
				shedule.t_max	= m_dwActiveScheduleMax;
			}
			//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
		};
		
		IC void vfRemoveActiveMember()
		{
			CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
			if (m_bActive) {
				R_ASSERT(Group.m_dwActiveCount > 0);
				--(Group.m_dwActiveCount);
				m_bActive = false;
				m_eCurrentState = aiZombieFreeHuntingPassive;
				shedule.t_min	= m_dwPassiveScheduleMin;
				shedule.t_max	= m_dwPassiveScheduleMax;
			}
			//Msg("* Group : alive[%2d], active[%2d]",Group.m_dwAliveCount,Group.m_dwActiveCount);
		};
		
		IC bool bfCheckIfSoundFrightful()
		{
			return(((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_HIT) == SOUND_TYPE_WEAPON_BULLET_HIT) || ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING));
		};
		
		IC bool bfCheckIfOutsideAIMap(Fvector &tTemp1)
		{
			u32 dwNewNode = level_vertex_id();
			const CLevelGraph::CVertex *tpNewNode = level_vertex();
			CLevelGraph::CPosition QueryPos;
			if (!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*level_vertex(),ai().level_graph().vertex_position(QueryPos,tTemp1)))
				tpNewNode = ai().level_graph().vertex(dwNewNode = ai().level_graph().vertex(level_vertex_id(),tTemp1));
			return(!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*tpNewNode,ai().level_graph().vertex_position(QueryPos,tTemp1)));
		};
		//////////////////////////
		// MISCELLANIOUS FUNCTIONS
		//////////////////////////
		void	vfAdjustSpeed();
		bool	bfComputeNewPosition(bool bCanAdjustSpeed = true, bool bStraightForward = false);
		void	vfLoadSounds();
		void	vfLoadAnimations();
		void	SetDirectionLook();
		void	vfAimAtEnemy();
		void	vfSetFire(bool bFire, CGroup &Group);
		void	vfSetMovementType(float fSpeed);
		void	vfUpdateDynamicObjects() {};
		//////////////////////////
		// FSM STATES
		//////////////////////////
		void	Death();
		void	FreeHuntingActive();
		void	FreeHuntingPassive();
		void	AttackFire();
		void	AttackRun();
		void	Turn();
		void	Pursuit();
		void	ReturnHome();
		void	Resurrect();
	public:
					   CAI_Zombie();
		virtual		  ~CAI_Zombie();
		virtual BOOL  renderable_ShadowReceive	()			{ return TRUE;	}
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element);
		virtual void  Die();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Action			(float dt);
		virtual	void  feel_sound_new		(CObject* who, int type, Fvector& Position, float power);
		virtual BOOL  feel_vision_isRelevant(CObject* who);
};
		
#endif
