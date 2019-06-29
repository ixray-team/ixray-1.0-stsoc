////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie.cpp
//	Created 	: 23.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "../ai_monsters_misc.h"
#include "../../xrserver_objects_alife_monsters.h"

CAI_Zombie::CAI_Zombie()
{
	m_hit_direction.set			(0,0,1);
	m_tSavedEnemyPosition.set(0,0,0);
	m_hit_time				= 0;
	m_tSavedEnemy			= 0;
	m_tpSavedEnemyNode		= 0;
	m_dwSavedEnemyNodeID	= u32(-1);
	m_dwLostEnemyTime		= 0;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend	= 0;
	m_bActionStarted		= false;
	m_bFiring				= false;
	m_dwLastVoiceTalk		= 0;
	m_dwLastPursuitTalk		= 0;
	m_tpSoundBeingPlayed	= 0;
	m_dwLastSoundRefresh	= 0;
	m_previous_query_time		= 0;
	m_tCurrentDir.set		(0,0,1);
	m_tHPB.set				(0,0,0);
	m_fDHeading				= 0;
	m_fGoalChangeTime		= 0.f;
	m_tLastSound.tpEntity	= 0;
	m_tLastSound.dwTime		= 0;
	m_tLastSound.eSoundType	= SOUND_TYPE_NO_SOUND;
	m_bNoWay				= false;
	m_bActive				= false;
	m_dwStartAttackTime		= 0;
	m_fLookSpeed			= PI;
	m_dwTimeToLie			= 6000;
	m_dwToWaitBeforeDestroy = 10000;
	m_tAction				= eZombieActionNone;
	inherited::Init			();
}

CAI_Zombie::~CAI_Zombie()
{
	DELETE_SOUNDS(SND_ATTACK_COUNT,		m_tpaSoundAttack	);
	DELETE_SOUNDS(SND_DEATH_COUNT,		m_tpaSoundDeath		);
	DELETE_SOUNDS(SND_HIT_COUNT,		m_tpaSoundHit		);
	DELETE_SOUNDS(SND_IDLE_COUNT,		m_tpaSoundIdle		);
	DELETE_SOUNDS(SND_NOTICE_COUNT,		m_tpaSoundNotice	);
	DELETE_SOUNDS(SND_PURSUIT_COUNT,	m_tpaSoundPursuit	);
	DELETE_SOUNDS(SND_RESURRECT_COUNT,	m_tpaSoundResurrect	);
}

void CAI_Zombie::Die()
{
	inherited::Die( );
	m_eCurrentState = aiZombieDie;
	
	///Fvector	dir;
	//direction(dir);
	//SelectAnimation(XFORM().k,dir,speed());
	
	::Sound->play_at_pos(m_tpaSoundDeath[Random.randI(SND_DEATH_COUNT)],this,Position());

	CGroup &Group = Level().get_group(g_Team(),g_Squad(),g_Group());
	vfRemoveActiveMember();
	--(Group.m_dwAliveCount);
	m_eCurrentState = aiZombieDie;
//	Msg("%s : Death signal %d",*cName(),Level().timeServer());
}

void CAI_Zombie::Load(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load(section);
	
	// initialize start position
	Fvector	P						= Position();
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();
	
	vfLoadSounds();
	
	// sounds
	m_fMinVoiceIinterval			= pSettings->r_float (section,"MinVoiceInterval");
	m_fMaxVoiceIinterval			= pSettings->r_float (section,"MaxVoiceInterval");
	m_fVoiceRefreshRate				= pSettings->r_float (section,"VoiceRefreshRate");

	m_fMinPursuitIinterval			= pSettings->r_float (section,"MinPursuitInterval");
	m_fMaxPursuitIinterval			= pSettings->r_float (section,"MaxPursuitInterval");
	m_fPursuitRefreshRate			= pSettings->r_float (section,"PursuitRefreshRate");

	// active\passive
	m_fChangeActiveStateProbability = pSettings->r_float (section,"ChangeActiveStateProbability");
	m_dwPassiveScheduleMin			= pSettings->r_s32   (section,"PassiveScheduleMin");
	m_dwPassiveScheduleMax			= pSettings->r_s32   (section,"PassiveScheduleMax");
	m_dwActiveCountPercent			= pSettings->r_s32   (section,"ActiveCountPercent");

	// eye shift
	m_tEyeShift.y					= pSettings->r_float (section,"EyeYShift");

	// former constants
	m_dwLostMemoryTime				= pSettings->r_s32   (section,"LostMemoryTime");
	m_fAttackStraightDistance		= pSettings->r_float (section,"AttackStraightDistance");
	m_fStableDistance				= pSettings->r_float (section,"StableDistance");
	m_fWallMinTurnValue				= pSettings->r_float (section,"WallMinTurnValue")/180.f*PI;
	m_fWallMaxTurnValue				= pSettings->r_float (section,"WallMaxTurnValue")/180.f*PI;

	m_fAngleSpeed					= pSettings->r_float (section,"AngleSpeed");
	m_fSafeGoalChangeDelta	 		= pSettings->r_float (section,"GoalChangeDelta");
	m_tGoalVariation	  			= pSettings->r_fvector3(section,"GoalVariation");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");
	m_fMaxHealthValue	 			= pSettings->r_float (section,"MaxHealthValue");

	m_dwActiveScheduleMin			= shedule.t_min;
	m_dwActiveScheduleMax			= shedule.t_max;
}

BOOL CAI_Zombie::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	//////////////////////////////////////////////////////////////////////////
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterZombie			*tpSE_Zombie = dynamic_cast<CSE_ALifeMonsterZombie*>(e);
	// model
	// personal characteristics
	m_body.current.yaw				= m_body.target.yaw	= -tpSE_Zombie->o_Angle.y;
	m_body.current.pitch			= m_body.target.pitch	= 0;

	eye_fov							= tpSE_Zombie->fEyeFov;
	eye_range						= tpSE_Zombie->fEyeRange;
	fEntityHealth							= tpSE_Zombie->fHealth;
	m_fMinSpeed						= tpSE_Zombie->fMinSpeed;
	m_fMaxSpeed						= tpSE_Zombie->fMaxSpeed;
	m_fAttackSpeed					= tpSE_Zombie->fAttackSpeed;
	m_fMaxPursuitRadius				= tpSE_Zombie->fMaxPursuitRadius;
	m_fMaxHomeRadius				= tpSE_Zombie->fMaxHomeRadius;
	// attack
	m_fHitPower						= tpSE_Zombie->fHitPower;
	m_dwHitInterval					= tpSE_Zombie->u16HitInterval;
	m_fAttackDistance				= tpSE_Zombie->fAttackDistance;
	m_fAttackAngle					= tpSE_Zombie->fAttackAngle/180.f*PI;
	//////////////////////////////////////////////////////////////////////////

	m_fCurSpeed						= m_fMaxSpeed;

	m_tOldPosition.set(Position());
	m_tSpawnPosition.set(Level().get_squad(g_Team(),g_Squad()).Leader->Position());
	m_tSafeSpawnPosition.set(m_tSpawnPosition);
	m_tStateStack.push(m_eCurrentState = aiZombieFreeHuntingActive);
	vfAddActiveMember(true);
	m_bStateChanged = true;

	m_tHPB.x = m_body.current.yaw;
	m_tHPB.y = m_body.current.pitch;
	m_tHPB.z = 0;
	
	vfLoadAnimations	();

	return TRUE;
}

void CAI_Zombie::net_Export(NET_Packet& P)
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fEntityHealth,-500,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
	P.w_u8					(u8(g_Team()));
	P.w_u8					(u8(g_Squad()));
	P.w_u8					(u8(g_Group()));
}

void CAI_Zombie::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());

	net_update				N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	id_Team					= P.r_u8();
	id_Squad				= P.r_u8();
	id_Group				= P.r_u8();

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}
