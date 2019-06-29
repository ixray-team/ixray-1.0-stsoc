////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_actions.h
//	Created 	: 30.09.2003
//  Modified 	: 30.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Script actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"
#include "car.h"
#include "movement_manager.h"
#include "script_sound.h"
#include "sight_manager_space.h"

class CAbstractAction {
public:
	bool							m_bCompleted;

							CAbstractAction		()
	{
		m_bCompleted		= true;
	}

	virtual					~CAbstractAction	()
	{
	}

	virtual	bool			completed			()
	{
		return				(m_bCompleted);
	}
};

class CPatrolPathParams {
public:
	const CPatrolPath						*m_path;
	ref_str									m_path_name;
	CPatrolPathManager::EPatrolStartType	m_tPatrolPathStart;
	CPatrolPathManager::EPatrolRouteType	m_tPatrolPathStop;
	bool									m_bRandom;
	u32										m_previous_index;

							CPatrolPathParams	(LPCSTR caPatrolPathToGo, const CPatrolPathManager::EPatrolStartType tPatrolPathStart = CPatrolPathManager::ePatrolStartTypeNearest, const CPatrolPathManager::EPatrolRouteType tPatrolPathStop = CPatrolPathManager::ePatrolRouteTypeContinue, bool bRandom = true, u32 index = u32(-1))
	{
		m_path_name			= caPatrolPathToGo;
		m_path				= Level().patrol_paths().path(ref_str(caPatrolPathToGo));
		m_tPatrolPathStart	= tPatrolPathStart;
		m_tPatrolPathStop	= tPatrolPathStop;
		m_bRandom			= bRandom;
		m_previous_index	= index;
	}

	virtual					~CPatrolPathParams	()
	{
	}

	IC	u32					count				() const
	{
		return				(m_path->vertices().size());
	}

	IC	const Fvector		&point				(u32 index) const
	{
		VERIFY				(m_path);
		VERIFY				(!m_path->vertices().empty());
		if (!m_path->vertex(index)) {
			ai().script_engine().script_log(eLuaMessageTypeError,"Can't get information about patrol point number %d in the patrol way %s",index,*m_path_name);
			index			= m_path->vertices().begin()->vertex_id();
		}
		VERIFY				(m_path->vertex(index));
		return				(m_path->vertex(index)->data().position());
	}

	IC	u32					level_vertex_id		(u32 index) const
	{
		VERIFY				(m_path->vertex(index));
		return				(m_path->vertex(index)->data().level_vertex_id());
	}

	IC	u32					point				(LPCSTR name) const
	{
		if (m_path->point(name))
			return			(m_path->point(name)->vertex_id());
		return				(u32(-1));
	}

	IC	u32					point				(const Fvector &point) const
	{
		return				(m_path->point(point)->vertex_id());
	}

	IC	bool				flag				(u32 index, u8 flag_index) const
	{
		VERIFY				(m_path->vertex(index));
		return				(!!(m_path->vertex(index)->data().flags() & (u32(1) << flag_index)));
	}
};

class CLuaGameObject;

class CScriptMovementAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypePatrolPath,
		eGoalTypePathPosition,
		eGoalTypeNoPathPosition,
		eGoalTypeInput,
		eGoalTypeDummy = u32(-1),
	};

	enum EInputKeys {
		eInputKeyNone		= u32(1) << 0,
		eInputKeyForward	= u32(1) << 1,
		eInputKeyBack		= u32(1) << 2,
		eInputKeyLeft		= u32(1) << 3,
		eInputKeyRight		= u32(1) << 4,
		eInputKeyShiftUp	= u32(1) << 5,
		eInputKeyShiftDown	= u32(1) << 6,
		eInputKeyBreaks		= u32(1) << 7,
		eInputKeyEngineOn	= u32(1) << 8,
		eInputKeyEngineOff	= u32(1) << 9,
		eInputKeyDummy		= u32(1) << 10,
	};

	ref_str							m_path_name;
	MonsterSpace::EBodyState		m_tBodyState;
	MonsterSpace::EMovementType		m_tMovementType;
	CDetailPathManager::EDetailPathType		m_tPathType;
	CObject							*m_tpObjectToGo;
	const CPatrolPath				*m_path;
	CPatrolPathManager::EPatrolStartType	m_tPatrolPathStart;
	CPatrolPathManager::EPatrolRouteType	m_tPatrolPathStop;
	Fvector							m_tDestinationPosition;
	u32								m_tNodeID;
	EGoalType						m_tGoalType;
	float							m_fSpeed;
	bool							m_bRandom;
	EInputKeys						m_tInputKeys;
	
	MonsterSpace::EScriptMonsterMoveAction		m_tMoveAction;
	MonsterSpace::EScriptMonsterSpeedParam		m_tSpeedParam;
	u32											m_previous_patrol_point;


							CScriptMovementAction		()
	{
		SetInputKeys		(eInputKeyNone);
		SetBodyState		(MonsterSpace::eBodyStateStand);
		SetMovementType		(MonsterSpace::eMovementTypeStand);
		SetPathType			(CDetailPathManager::eDetailPathTypeSmooth);
		SetPatrolPath		(0,"");
		SetPatrolStart		(CPatrolPathManager::ePatrolStartTypeNearest);
		SetPatrolStop		(CPatrolPathManager::ePatrolRouteTypeContinue);
		SetPatrolRandom		(true);
		SetSpeed			(0);
		SetObjectToGo		(0);
		SetPosition			(Fvector().set(0,0,0));
		m_tGoalType			= eGoalTypeDummy;
		m_bCompleted		= true;
	}

							CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, CDetailPathManager::EDetailPathType tPathType, CLuaGameObject *tpObjectToGo, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetObjectToGo		(tpObjectToGo);
		SetSpeed			(fSpeed);
	}

							CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, CDetailPathManager::EDetailPathType tPathType, const CPatrolPathParams &tPatrolPathParams, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPatrolPath		(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);
		SetPatrolStart		(tPatrolPathParams.m_tPatrolPathStart);
		SetPatrolStop		(tPatrolPathParams.m_tPatrolPathStop);
		SetPatrolRandom		(tPatrolPathParams.m_bRandom);
		SetSpeed			(fSpeed);
	}

							CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, CDetailPathManager::EDetailPathType tPathType, const Fvector &tPosition, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPosition			(tPosition);
		SetSpeed			(fSpeed);
	}

							CScriptMovementAction		(const Fvector &tPosition, float fSpeed)
	{
		SetBodyState		(MonsterSpace::eBodyStateStand);
		SetMovementType		(MonsterSpace::eMovementTypeStand);
		SetPathType			(CDetailPathManager::eDetailPathTypeSmooth);
		SetPosition			(tPosition);
		SetSpeed			(fSpeed);
		m_tGoalType			= eGoalTypeNoPathPosition;
	}

							CScriptMovementAction		(const EInputKeys tInputKeys, float fSpeed = 0.f)
	{
		SetInputKeys		(tInputKeys);
		SetSpeed			(fSpeed);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------						
	// Monsters
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
							CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector &tPosition, MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default)
	{																																			
		m_tMoveAction		= tAct;
		SetPosition			(tPosition);																										
		m_tSpeedParam		= speed_param;
	}																																			
							CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default)
	{																																			
		m_tMoveAction			= tAct;
		SetPatrolPath			(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);															
		SetPatrolStart			(tPatrolPathParams.m_tPatrolPathStart);																				
		SetPatrolStop			(tPatrolPathParams.m_tPatrolPathStop);																				
		SetPatrolRandom			(tPatrolPathParams.m_bRandom);																						
		m_tSpeedParam			= speed_param;
		m_previous_patrol_point = tPatrolPathParams.m_previous_index;
	}																																			
							CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CLuaGameObject *tpObjectToGo, MonsterSpace::EScriptMonsterSpeedParam speed_param = MonsterSpace::eSP_Default)
	{
		m_tMoveAction		= tAct;
		SetObjectToGo		(tpObjectToGo);
		m_tSpeedParam		= speed_param;
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

			void			SetBodyState		(const MonsterSpace::EBodyState tBodyState)
	{
		m_tBodyState		= tBodyState;
		m_bCompleted		= false;
	}

			void			SetMovementType		(const MonsterSpace::EMovementType tMovementType)
	{
		m_tMovementType		= tMovementType;
		m_bCompleted		= false;
	}

			void			SetPathType			(const CDetailPathManager::EDetailPathType tPathType)
	{
		m_tPathType			= tPathType;
		m_bCompleted		= false;
	}

			void			SetObjectToGo		(CLuaGameObject *tpObjectToGo);

			void			SetPatrolPath		(const CPatrolPath *path, ref_str path_name)
	{
		m_path				= path;
		m_path_name			= path_name;
		m_tGoalType			= eGoalTypePatrolPath;
		m_bCompleted		= false;
	}

			void			SetPosition			(const Fvector &tPosition)
	{
		m_tDestinationPosition = tPosition;
		m_tGoalType			= eGoalTypePathPosition;
		m_bCompleted		= false;
	}

			void			SetSpeed			(float fSpeed)
	{
		m_fSpeed			= fSpeed;
		m_bCompleted		= false;
	}

			void			SetPatrolStart		(CPatrolPathManager::EPatrolStartType tPatrolPathStart)
	{
		m_tPatrolPathStart	= tPatrolPathStart;
		m_bCompleted		= false;
	}

			void			SetPatrolStop		(CPatrolPathManager::EPatrolRouteType tPatrolPathStop)
	{
		m_tPatrolPathStop	= tPatrolPathStop;
		m_bCompleted		= false;
	}

			void			SetPatrolRandom		(bool bRandom)
	{
		m_bRandom			= bRandom;
		m_bCompleted		= false;
	}

			void			SetInputKeys		(const EInputKeys tInputKeys)
	{
		m_tInputKeys		= tInputKeys;
		m_tGoalType			= eGoalTypeInput;
		m_bCompleted		= false;
	}

	IC		void			initialize				()
	{
	}
};

class CScriptWatchAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypeWatchType,
		eGoalTypeDirection,
		eGoalTypeCurrent,
		eGoalTypeDummy = u32(-1),
	};
	CObject							*m_tpObjectToWatch;
	SightManager::ESightType		m_tWatchType;
	EGoalType						m_tGoalType;
	Fvector							m_tWatchVector;
	ref_str							m_bone_to_watch;

	// Searchlight 
	Fvector								m_tTargetPoint;
	float								vel_bone_x;
	float								vel_bone_y;


							CScriptWatchAction		()
	{
		m_tpObjectToWatch	= 0;
		m_tWatchType		= SightManager::eSightTypeCurrentDirection;
		m_tWatchVector.set	(0,0,0);
		m_tGoalType			= eGoalTypeCurrent;
		m_bCompleted		= true;
	}

							CScriptWatchAction		(SightManager::ESightType tWatchType)
	{
		SetWatchType		(tWatchType);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CScriptWatchAction		(SightManager::ESightType tWatchType, const Fvector &tDirection)
	{
		SetWatchDirection	(tDirection);
		SetWatchType		(tWatchType);
	}

							CScriptWatchAction		(SightManager::ESightType tWatchType, CLuaGameObject *tpObjectToWatch, LPCSTR bone_to_watch = "")
	{
		SetWatchType		(tWatchType);
		SetWatchObject		(tpObjectToWatch);
		SetWatchBone		(bone_to_watch);
	}

	// Searchlight look ///////////////////////////////////////////////
							CScriptWatchAction		(const Fvector &tTarget, float vel1, float vel2)
	{
		m_tpObjectToWatch	= 0;
		m_tTargetPoint		= tTarget;
		vel_bone_x			= vel1;
		vel_bone_y			= vel2;
		m_bCompleted		= false;
	}

							CScriptWatchAction		(CLuaGameObject *tpObjectToWatch, float vel1, float vel2)
	{
		SetWatchObject		(tpObjectToWatch);
		vel_bone_x			= vel1;
		vel_bone_y			= vel2;
		m_bCompleted		= false;
	}
	
	///////////////////////////////////////////////////////////////////
	
	
			void			SetWatchObject		(CLuaGameObject *tpObjectToWatch);

			void			SetWatchType		(SightManager::ESightType tWatchType)
	{
		m_tWatchType		= tWatchType;
		m_bCompleted		= false;
	}

			void			SetWatchDirection	(const Fvector &tDirection)
	{
		m_tWatchVector		= tDirection;
		m_tGoalType			= eGoalTypeDirection;
		m_bCompleted		= false;
	}

			void			SetWatchBone		(LPCSTR bone_to_watch)
	{
		m_bone_to_watch		= bone_to_watch;
		m_bCompleted		= false;
	}

	IC		void			initialize				()
	{
	}
};

class CScriptAnimationAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeAnimation = u32(0),
		eGoalTypeMental,
		eGoalTypeDummy = u32(-1),
	};
	ref_str							m_caAnimationToPlay;
	MonsterSpace::EMentalState		m_tMentalState;
	EGoalType						m_tGoalType;
	bool							m_bHandUsage;
	
	MonsterSpace::EScriptMonsterAnimAction	m_tAnimAction;
	int										anim_index;


							CScriptAnimationAction	()
	{
		m_tMentalState		= MonsterSpace::eMentalStateDanger;
		m_tGoalType			= eGoalTypeMental;
		m_bHandUsage		= true;
		m_tAnimAction		= MonsterSpace::eAA_NoAction;
		m_bCompleted		= true;
	}

							CScriptAnimationAction	(LPCSTR caAnimationToPlay, bool use_single_hand = false)
	{
		SetAnimation		(caAnimationToPlay);
		m_bHandUsage		= !use_single_hand;
	}

							CScriptAnimationAction	(MonsterSpace::EMentalState tMentalState)
	{
		SetMentalState		(tMentalState);
	}

	// -------------------------------------------------------------------------------------------------
	// Monster
	// -------------------------------------------------------------------------------------------------
							CScriptAnimationAction	(MonsterSpace::EScriptMonsterAnimAction tAnimAction, int index)
	{
		m_tAnimAction		= tAnimAction;
		m_bCompleted		= false;
		anim_index			= index;
	}
	// -------------------------------------------------------------------------------------------------


			void			SetAnimation		(LPCSTR caAnimationToPlay)
	{
		m_caAnimationToPlay	= caAnimationToPlay;
		m_tMentalState		= MonsterSpace::eMentalStateDanger;
		m_tGoalType			= eGoalTypeAnimation;
		m_bCompleted		= false;
	}

			void			SetMentalState		(MonsterSpace::EMentalState tMentalState)
	{
		m_tMentalState		= tMentalState;
		m_tGoalType			= eGoalTypeMental;
		m_bCompleted		= true;
	}


	IC		void			initialize				()
	{
	}
};

class CScriptSoundAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeSoundAttached = u32(0),
		eGoalTypeSoundPosition,
		eGoalTypeDummy = u32(-1),
	};
	ref_str							m_caSoundToPlay;
	ref_str							m_caBoneName;
	EGoalType						m_tGoalType;
	bool							m_bLooped;
	bool							m_bStartedToPlay;
	Fvector							m_tSoundPosition;
	Fvector							m_tSoundAngles;
	ESoundTypes						m_sound_type;
	
	MonsterSpace::EMonsterSounds	m_monster_sound;
	int								m_monster_sound_delay;
	
	MonsterSpace::EMonsterHeadAnimType m_tHeadAnimType;


							CScriptSoundAction		()
	{
		m_caSoundToPlay		= "";
		m_caBoneName		= "";
		m_tGoalType			= eGoalTypeDummy;
		m_bStartedToPlay	= false;
		m_bLooped			= false;
		m_tSoundPosition.set(0,0,0);
		m_tSoundAngles.set	(0,0,0);
		m_bCompleted		= true;
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
		m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
	}

							CScriptSoundAction		(LPCSTR caSoundToPlay, LPCSTR caBoneName, const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND)
	{
		m_bLooped			= bLooped;
		SetBone				(caBoneName);
		SetPosition			(tPositionOffset);
		SetAngles			(tAngleOffset);
		SetSound			(caSoundToPlay);
		SetSoundType		(sound_type);
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
		m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
	}

							CScriptSoundAction		(LPCSTR caSoundToPlay, const Fvector &tPosition, const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND)
	{
		m_bLooped			= bLooped;
		SetSound			(caSoundToPlay);
		SetPosition			(tPosition);
		SetAngles			(tAngleOffset);
		SetSoundType		(sound_type);
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
		m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
	}

							CScriptSoundAction		(CScriptSound &sound, LPCSTR caBoneName, const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND)
	{
		m_bLooped			= bLooped;
		SetBone				(caBoneName);
		SetPosition			(tPositionOffset);
		SetAngles			(tAngleOffset);
		SetSound			(sound);
		SetSoundType		(sound_type);
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
		m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
	}

							CScriptSoundAction		(CScriptSound &sound, const Fvector &tPosition, const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND)
	{
		m_bLooped			= bLooped;
		SetSound			(sound);
		SetPosition			(tPosition);
		SetAngles			(tAngleOffset);
		SetSoundType		(sound_type);
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
		m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
	}
	
	////////////////////////////////////////////////////////////////////////////////////
	// Monster Specific
	///////////////////////////////////////////////////////////////////////////////////
							
							CScriptSoundAction		(MonsterSpace::EMonsterSounds sound_type)
	{
		m_monster_sound			= sound_type;
		m_monster_sound_delay	= int (-1);
		m_bCompleted			= false;
	}

							CScriptSoundAction		(MonsterSpace::EMonsterSounds sound_type, int delay)
	{
		m_monster_sound			= sound_type;
		m_monster_sound_delay	= delay;
		m_bCompleted			= false;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Trader Specific
	///////////////////////////////////////////////////////////////////////////////////

							CScriptSoundAction		(LPCSTR caSoundToPlay, LPCSTR caBoneName, MonsterSpace::EMonsterHeadAnimType head_anim_type)
	{
		SetBone				(caBoneName);
		SetSound			(caSoundToPlay);		
		m_tSoundPosition.set(0,0,0);
		m_tSoundAngles.set	(0,0,0);
		m_bCompleted		= false;
		m_bLooped			= false;
		m_tHeadAnimType		= head_anim_type;
		m_monster_sound		= MonsterSpace::eMonsterSoundDummy;
	}

	///////////////////////////////////////////////////////////////////////////////////


	virtual					~CScriptSoundAction		()
	{
	}

			void			SetSound			(LPCSTR caSoundToPlay)
	{
		m_caSoundToPlay		= caSoundToPlay;
		m_tGoalType			= eGoalTypeSoundAttached;
		m_bStartedToPlay	= false;
		string256			l_caFileName;
		if (FS.exist(l_caFileName,"$game_sounds$",*m_caSoundToPlay,".ogg")) {
			m_bStartedToPlay= false;
			m_bCompleted	= false;
		}
		else {
			ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
			m_bStartedToPlay= true;
			m_bCompleted	= true;
		}
	}

			void			SetSound			(const CScriptSound &sound)
	{
		m_caSoundToPlay		= sound.m_caSoundToPlay;
		m_tGoalType			= eGoalTypeSoundAttached;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetPosition			(const Fvector &tPosition)
	{
		m_tSoundPosition	= tPosition;
		m_tGoalType			= eGoalTypeSoundPosition;
		m_bStartedToPlay	= false;
	}

			void			SetBone				(LPCSTR caBoneName)
	{
		m_caBoneName		= caBoneName;
		m_bStartedToPlay	= false;
	}
	
	void			SetAngles			(const Fvector &tAngles)
	{
		m_tSoundAngles		= tAngles;
		m_bStartedToPlay	= false;
	}

	void			SetSoundType		(const ESoundTypes sound_type)
	{
		m_sound_type		= sound_type;
		m_bStartedToPlay	= false;
	}

	IC		void			initialize				()
	{
		m_bStartedToPlay	= false;
	}
};

class CParticleParams {
public:
	Fvector							m_tParticlePosition;
	Fvector							m_tParticleAngles;
	Fvector							m_tParticleVelocity;

							CParticleParams(const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAnglesOffset = Fvector().set(0,0,0), const Fvector &tVelocity = Fvector().set(0,0,0))
	{
		m_tParticlePosition	= tPositionOffset;
		m_tParticleAngles	= tAnglesOffset;
		m_tParticleVelocity	= tVelocity;
	}

	virtual					~CParticleParams()
	{
	}

	IC		void			initialize				()
	{
	}
};

class CScriptParticleAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeParticleAttached = u32(0),
		eGoalTypeParticlePosition,
		eGoalTypeDummy = u32(-1),
	};
	ref_str							m_caParticleToRun;
	ref_str							m_caBoneName;
	EGoalType						m_tGoalType;
	CParticlesObject				*m_tpParticleSystem;
	bool							m_bStartedToPlay;
	Fvector							m_tParticlePosition;
	Fvector							m_tParticleAngles;
	Fvector							m_tParticleVelocity;
	bool							m_bAutoRemove;

							CScriptParticleAction		()
	{
		m_caParticleToRun	= "";
		m_caBoneName		= "";
		m_tGoalType			= eGoalTypeDummy;
		m_bCompleted		= false;
		m_bStartedToPlay	= false;
		m_tpParticleSystem	= 0;
		m_tParticlePosition.set	(0,0,0);
		m_tParticleAngles.set	(0,0,0);
		m_tParticleVelocity.set	(0,0,0);
		m_bAutoRemove		= true;
	}

							CScriptParticleAction		(LPCSTR caPartcileToRun, LPCSTR caBoneName, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false)
	{
		SetBone				(caBoneName);
		SetPosition			(tParticleParams.m_tParticlePosition);
		SetAngles			(tParticleParams.m_tParticleAngles);
		SetVelocity			(tParticleParams.m_tParticleVelocity);
		SetParticle			(caPartcileToRun,bAutoRemove);
	}

							CScriptParticleAction		(LPCSTR caPartcileToRun, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false)
	{
		SetParticle			(caPartcileToRun,bAutoRemove);
		SetPosition			(tParticleParams.m_tParticlePosition);
		SetAngles			(tParticleParams.m_tParticleAngles);
		SetVelocity			(tParticleParams.m_tParticleVelocity);
	}

	virtual					~CScriptParticleAction	();

			void			SetParticle			(LPCSTR caParticleToRun, bool bAutoRemove);
			
			void			SetPosition			(const Fvector &tPosition)
	{
		m_tParticlePosition	= tPosition;
		m_tGoalType			= eGoalTypeParticlePosition;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetBone				(LPCSTR caBoneName)
	{
		m_caBoneName		= caBoneName;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetAngles			(const Fvector &tAngleOffset)
	{
		m_tParticleAngles	= tAngleOffset;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetVelocity			(const Fvector &tVelocity)
	{
		m_tParticleVelocity	= tVelocity;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

	IC		void			initialize				()
	{
	}
};

class CScriptObjectAction : public CAbstractAction {
public:
	CObject							*m_tpObject;
	MonsterSpace::EObjectAction		m_tGoalType;
	u32								m_dwQueueSize;
	ref_str							m_caBoneName;

							CScriptObjectAction	()
	{
		m_tpObject			= 0;
		m_tGoalType			= MonsterSpace::eObjectActionIdle;
		m_bCompleted		= true;
	}

							CScriptObjectAction	(CLuaGameObject *tpLuaGameObject, MonsterSpace::EObjectAction tObjectActionType, u32 dwQueueSize = u32(-1))
	{
		SetObject			(tpLuaGameObject);
		SetObjectAction		(tObjectActionType);
		SetQueueSize		(dwQueueSize);
	}

							CScriptObjectAction	(LPCSTR caBoneName, MonsterSpace::EObjectAction tObjectActionType)
	{
		SetObject			(caBoneName);
		SetObjectAction		(tObjectActionType);
	}

							CScriptObjectAction	(MonsterSpace::EObjectAction tObjectActionType)
	{
		SetObjectAction		(tObjectActionType);
	}



			void			SetObject			(CLuaGameObject *tpLuaGameObject);

			void			SetObject			(LPCSTR	caBoneName)
	{
		m_caBoneName		= caBoneName;
		m_bCompleted		= false;
	}

			void			SetObjectAction		(MonsterSpace::EObjectAction tObjectActionType)
	{
		m_tGoalType			= tObjectActionType;
		m_bCompleted		= false;
	}

			void			SetQueueSize		(u32 dwQueueSize)
	{
		m_dwQueueSize		= dwQueueSize;
		m_bCompleted		= false;
	}

	IC		void			initialize				()
	{
	}
};

class CActionCondition {
public:
	enum EActionFlags {
		MOVEMENT_FLAG	= u32(1 << 0),
		WATCH_FLAG		= u32(1 << 1),
		ANIMATION_FLAG	= u32(1 << 2),
		SOUND_FLAG		= u32(1 << 3),
		PARTICLE_FLAG	= u32(1 << 4),
		OBJECT_FLAG		= u32(1 << 5),
		TIME_FLAG		= u32(1 << 6),
		ACT_FLAG		= u32(1 << 7)
	};
	u32								m_dwFlags;
	ALife::_TIME_ID						m_tLifeTime;
	ALife::_TIME_ID						m_tStartTime;
							CActionCondition		()
	{
		m_dwFlags			= 0;
		m_tLifeTime			= ALife::_TIME_ID(-1);
		m_tStartTime		= ALife::_TIME_ID(-1);
	}

							CActionCondition		(u32 dwFlags, double dTime = -1)
	{
		m_dwFlags			= dwFlags;
		m_tLifeTime			= ALife::_TIME_ID(dTime);
		m_tStartTime		= ALife::_TIME_ID(-1);
	}

	IC		void			initialize				()
	{
		m_tStartTime		= Device.TimerAsync();
	}

			bool			completed				() const
	{
	}
};

class CMonsterAction : public CAbstractAction {
public: 
	MonsterSpace::EScriptMonsterGlobalAction	m_tAction;
	CObject										*m_tObject;

	CMonsterAction() {
		m_tAction		= MonsterSpace::eGA_None;
		m_bCompleted	= true;
	}

	CMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action) {
		m_tAction		= action;
		m_bCompleted	= false;
	}
	CMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action, CLuaGameObject *tObj) {
		m_tAction		= action;
		m_bCompleted	= false;
		
		SetObject		(tObj);
	}

	void	SetObject(CLuaGameObject *tObj);
};



class CEntityAction {
public:
	CScriptMovementAction			m_tMovementAction;
	CScriptWatchAction				m_tWatchAction;
	CScriptAnimationAction			m_tAnimationAction;
	CScriptSoundAction				m_tSoundAction;
	CScriptParticleAction			m_tParticleAction;
	CScriptObjectAction				m_tObjectAction;
	CActionCondition				m_tActionCondition;
	CMonsterAction					m_tMonsterAction;

	void							*m_user_data;

							CEntityAction		()
	{
		m_user_data					= 0;
	}

							CEntityAction		(const CEntityAction *entity_action)
	{
		*this						= *entity_action;
	}

	virtual					~CEntityAction		()
	{
	}

	template<typename T>
	IC		void			SetAction(const T &t, T &tt)
	{
		tt					= t;
	}

	IC		void			SetAction(CScriptMovementAction &tMovementAction)
	{
		SetAction			(tMovementAction,m_tMovementAction);
	}

	IC		void			SetAction(CScriptWatchAction &tWatchAction)
	{
		SetAction			(tWatchAction,m_tWatchAction);
	}

	IC		void			SetAction(CScriptAnimationAction &tAnimationAction)
	{
		SetAction			(tAnimationAction,m_tAnimationAction);
	}

	IC		void			SetAction(CScriptSoundAction &tSoundAction)
	{
		SetAction			(tSoundAction,m_tSoundAction);
	}

	IC		void			SetAction(CScriptParticleAction &tParticleAction)
	{
		SetAction			(tParticleAction,m_tParticleAction);
	}

	IC		void			SetAction(CScriptObjectAction &tObjectAction)
	{
		SetAction			(tObjectAction,m_tObjectAction);
	}

	IC		void			SetAction(CActionCondition &tActionCondition)
	{
		SetAction			(tActionCondition,m_tActionCondition);
	}
	IC		void			SetAction(CMonsterAction &tMonsterAction)
	{
		SetAction			(tMonsterAction,m_tMonsterAction);
	}

	IC		void			SetAction(void *user_data)
	{
		m_user_data			= user_data;
	}

	IC		bool			CheckIfActionCompleted(const CAbstractAction &tAbstractAction) const
	{
		return				(tAbstractAction.m_bCompleted);
	}

	IC		bool			CheckIfMovementCompleted() const
	{
		return				(CheckIfActionCompleted(m_tMovementAction));
	}

	IC		bool			CheckIfWatchCompleted() const
	{
		return				(CheckIfActionCompleted(m_tWatchAction));
	}

	IC		bool			CheckIfAnimationCompleted() const
	{
		return				(CheckIfActionCompleted(m_tAnimationAction));
	}

	IC		bool			CheckIfSoundCompleted() const
	{
		return				(CheckIfActionCompleted(m_tSoundAction));
	}

	IC		bool			CheckIfParticleCompleted() const
	{
		return				(CheckIfActionCompleted(m_tParticleAction));
	}

	IC		bool			CheckIfObjectCompleted() const
	{
		return				(CheckIfActionCompleted(m_tObjectAction));
	}
	IC		bool			CheckIfMonsterActionCompleted() const
	{
		return				(CheckIfActionCompleted(m_tMonsterAction));
	}

	IC		bool			CheckIfTimeOver()
	{
		return((m_tActionCondition.m_tLifeTime >= 0) && ((m_tActionCondition.m_tStartTime + m_tActionCondition.m_tLifeTime) < Device.TimerAsync()));
	}

	IC		bool			CheckIfActionCompleted()
	{
		u32					l_dwFlags = m_tActionCondition.m_dwFlags;
		if ((CActionCondition::MOVEMENT_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfMovementCompleted	())
			l_dwFlags		^= CActionCondition::MOVEMENT_FLAG;
		if ((CActionCondition::WATCH_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfWatchCompleted	())
			l_dwFlags		^= CActionCondition::WATCH_FLAG;
		if ((CActionCondition::ANIMATION_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfAnimationCompleted())
			l_dwFlags		^= CActionCondition::ANIMATION_FLAG;
		if ((CActionCondition::SOUND_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfSoundCompleted	())
			l_dwFlags		^= CActionCondition::SOUND_FLAG;
		if ((CActionCondition::PARTICLE_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfParticleCompleted	())
			l_dwFlags		^= CActionCondition::PARTICLE_FLAG;
		if ((CActionCondition::OBJECT_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfObjectCompleted	())
			l_dwFlags		^= CActionCondition::OBJECT_FLAG;
		if ((CActionCondition::TIME_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfTimeOver			())
			l_dwFlags		^= CActionCondition::TIME_FLAG;
		if ((CActionCondition::ACT_FLAG			& m_tActionCondition.m_dwFlags)	&& CheckIfMonsterActionCompleted())
			l_dwFlags		^= CActionCondition::ACT_FLAG;

		if (!m_tActionCondition.m_dwFlags && (m_tActionCondition.m_tLifeTime < 0) 
			&& CheckIfMovementCompleted()
			&& CheckIfWatchCompleted()
			&& CheckIfAnimationCompleted()
			&& CheckIfSoundCompleted()
			&& CheckIfParticleCompleted()
			&& CheckIfObjectCompleted()
			&& CheckIfMonsterActionCompleted()
			)
			return			(true);
		else
			return			(!l_dwFlags);
	}

	IC		void			initialize	()
	{
		m_tMovementAction.initialize	();
		m_tWatchAction.initialize		();
		m_tAnimationAction.initialize	();
		m_tSoundAction.initialize		();
		m_tParticleAction.initialize	();
		m_tObjectAction.initialize		();
		m_tActionCondition.initialize	();
	}

			const CScriptMovementAction	&move()
	{
		return				(m_tMovementAction);
	}
	
			const CScriptWatchAction		&look()
	{
		return				(m_tWatchAction);
	}
	
			const CScriptAnimationAction	&anim()
	{
		return				(m_tAnimationAction);
	}
	
			const CScriptParticleAction	&particle()
	{
		return				(m_tParticleAction);
	}
	
			const CScriptObjectAction	&object()
	{
		return				(m_tObjectAction);
	}
	
			const CActionCondition	&cond()
	{
		return				(m_tActionCondition);
	}
	
			void					*data()
	{
		return				(m_user_data);
	}
};


