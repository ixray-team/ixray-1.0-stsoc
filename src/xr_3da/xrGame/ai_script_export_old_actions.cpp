////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_old_actions.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export old actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"
#include "script_sound.h"
#include "script_hit.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "script_engine.h"

using namespace luabind;

void CScriptEngine::export_actions()
{
	module(lua())
	[
		class_<CPatrolPathParams>("patrol")
			.enum_("start")
			[
				value("start",					int(CPatrolPathManager::ePatrolStartTypeFirst)),
				value("stop",					int(CPatrolPathManager::ePatrolStartTypeLast)),
				value("nearest",				int(CPatrolPathManager::ePatrolStartTypeNearest)),
				value("custom",					int(CPatrolPathManager::ePatrolStartTypePoint)),
				value("next",					int(CPatrolPathManager::ePatrolStartTypeNext)),
				value("dummy",					int(CPatrolPathManager::ePatrolStartTypeDummy))
			]
			.enum_("stop")
			[
				value("stop",					int(CPatrolPathManager::ePatrolRouteTypeStop)),
				value("continue",				int(CPatrolPathManager::ePatrolRouteTypeContinue)),
				value("dummy",					int(CPatrolPathManager::ePatrolRouteTypeDummy))
			]
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType, bool>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType, bool, u32>())
			.def("count",						&CPatrolPathParams::count)
			.def("level_vertex_id",				&CPatrolPathParams::level_vertex_id)
			.def("point",						(const Fvector &(CPatrolPathParams::*)(u32)				const)	(CPatrolPathParams::point))
			.def("index",						(u32			(CPatrolPathParams::*)(LPCSTR)			const)	(CPatrolPathParams::point))
			.def("get_nearest",					(u32			(CPatrolPathParams::*)(const Fvector &) const)	(CPatrolPathParams::point))
			.def("flag",						&CPatrolPathParams::flag),

		class_<CScriptMovementAction>("move")
			.enum_("body")
			[
				value("crouch",					int(MonsterSpace::eBodyStateCrouch)),
				value("standing",				int(MonsterSpace::eBodyStateStand))
			]
			.enum_("move")
			[
				value("walk",					int(MonsterSpace::eMovementTypeWalk)),
				value("run",					int(MonsterSpace::eMovementTypeRun)),
				value("stand",					int(MonsterSpace::eMovementTypeStand))
			]
			.enum_("path")
			[
				value("line",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("dodge",					int(CDetailPathManager::eDetailPathTypeSmoothDodge)),
				value("criteria",				int(CDetailPathManager::eDetailPathTypeSmoothCriteria)),
				value("curve",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("curve_criteria",			int(CDetailPathManager::eDetailPathTypeSmoothCriteria))
			]
			.enum_("input")
			[
				value("none",					int(CScriptMovementAction::eInputKeyNone)),
				value("fwd",					int(CScriptMovementAction::eInputKeyForward)),
				value("back",					int(CScriptMovementAction::eInputKeyBack)),
				value("left",					int(CScriptMovementAction::eInputKeyLeft)),
				value("right",					int(CScriptMovementAction::eInputKeyRight)),
				value("up",						int(CScriptMovementAction::eInputKeyShiftUp)),
				value("down",					int(CScriptMovementAction::eInputKeyShiftDown)),
				value("break",					int(CScriptMovementAction::eInputKeyBreaks)),
				value("on",						int(CScriptMovementAction::eInputKeyEngineOn)),
				value("off",					int(CScriptMovementAction::eInputKeyEngineOff))
			]
			.enum_("monster")
			[

				value("walk_fwd",					int(MonsterSpace::eMA_WalkFwd)),
				value("walk_bkwd",					int(MonsterSpace::eMA_WalkBkwd)),
				value("run_fwd",					int(MonsterSpace::eMA_Run)),
				value("drag",						int(MonsterSpace::eMA_Drag)),
				value("jump",						int(MonsterSpace::eMA_Jump)),
				value("steal",						int(MonsterSpace::eMA_Steal))
			]
			.enum_("monster_speed_param")
			[
				value("default",					int(MonsterSpace::eSP_Default)),
				value("force",						int(MonsterSpace::eSP_ForceSpeed))
			]

			.def(								constructor<>())
			.def(								constructor<const CScriptMovementAction::EInputKeys>())
			.def(								constructor<const CScriptMovementAction::EInputKeys, float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CLuaGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CLuaGameObject*,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &,float>())
			.def(								constructor<const Fvector &,float>())
			
			// Monsters 
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,Fvector &,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &,	MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CLuaGameObject*,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,Fvector &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CLuaGameObject*>())


			.def("body",						&CScriptMovementAction::SetBodyState)
			.def("move",						&CScriptMovementAction::SetMovementType)
			.def("path",						&CScriptMovementAction::SetPathType)
			.def("object",						&CScriptMovementAction::SetObjectToGo)
			.def("patrol",						&CScriptMovementAction::SetPatrolPath)
			.def("position",					&CScriptMovementAction::SetPosition)
			.def("input",						&CScriptMovementAction::SetInputKeys)
			.def("completed",					(bool (CScriptMovementAction::*)())(CScriptMovementAction::completed)),


		class_<CScriptWatchAction>("look")
			.enum_("look")
			[
				value("path_dir",				int(SightManager::eSightTypePathDirection)),
				value("search",					int(SightManager::eSightTypeSearch)),
				value("danger",					int(SightManager::eSightTypeCover)),
				value("point",					int(SightManager::eSightTypePosition)),
				value("fire_point",				int(SightManager::eSightTypeFirePosition)),
				value("cur_dir",				int(SightManager::eSightTypeCurrentDirection)),
				value("direction",				int(SightManager::eSightTypeDirection))
			]
			.def(								constructor<>())
			.def(								constructor<SightManager::ESightType>())
			.def(								constructor<SightManager::ESightType, const Fvector &>())
			.def(								constructor<SightManager::ESightType, CLuaGameObject*>())
			.def(								constructor<SightManager::ESightType, CLuaGameObject*, LPCSTR>())

			// searchlight 
			.def(								constructor<const Fvector &,float,float>())
			.def(								constructor<CLuaGameObject*,float,float>())

			.def("object",						&CScriptWatchAction::SetWatchObject)		// time
			.def("direct",						&CScriptWatchAction::SetWatchDirection)		// time
			.def("type",						&CScriptWatchAction::SetWatchType)
			.def("bone",						&CScriptWatchAction::SetWatchBone)
			.def("completed",					(bool (CScriptWatchAction::*)())(CScriptWatchAction::completed)),

		class_<CScriptAnimationAction>("anim")
			.enum_("type")
			[
				value("free",					int(MonsterSpace::eMentalStateFree)),
				value("danger",					int(MonsterSpace::eMentalStateDanger)),
				value("asleep",					int(MonsterSpace::eMentalStateAsleep)),
				value("zombied",				int(MonsterSpace::eMentalStateZombied)),
				value("dummy",					int(MonsterSpace::eMentalStateDummy))
			]
			.enum_("monster")
			[
				value("stand_idle",				int(MonsterSpace::eAA_StandIdle)),
				value("sit_idle",				int(MonsterSpace::eAA_SitIdle)),
				value("lie_idle",				int(MonsterSpace::eAA_LieIdle)),
				value("eat",					int(MonsterSpace::eAA_Eat)),
				value("sleep",					int(MonsterSpace::eAA_Sleep)),
				value("rest",					int(MonsterSpace::eAA_Rest)),
				value("attack",					int(MonsterSpace::eAA_Attack)),
				value("look_around",			int(MonsterSpace::eAA_LookAround)),
				value("turn",					int(MonsterSpace::eAA_Turn))
			]

			.def(								constructor<>())
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,bool>())
			.def(								constructor<MonsterSpace::EMentalState>())
			
			// Monster specific
			.def(								constructor<MonsterSpace::EScriptMonsterAnimAction, int>())
			
			.def("anim",						&CScriptAnimationAction::SetAnimation)
			.def("type",						&CScriptAnimationAction::SetMentalState)
			.def("completed",					(bool (CScriptAnimationAction::*)())(CScriptAnimationAction::completed)),

		class_<CScriptSoundAction>("sound")
			.enum_("type")
			[
				value("idle",					int(MonsterSpace::eMonsterSoundIdle)),
				value("eat",					int(MonsterSpace::eMonsterSoundEat)),
				value("attack",					int(MonsterSpace::eMonsterSoundAttack)),
				value("attack_hit",				int(MonsterSpace::eMonsterSoundAttackHit)),
				value("take_damage",			int(MonsterSpace::eMonsterSoundTakeDamage)),
				value("die",					int(MonsterSpace::eMonsterSoundDie)),
				value("threaten",				int(MonsterSpace::eMonsterSoundThreaten)),
				value("steal",					int(MonsterSpace::eMonsterSoundSteal)),
				value("panic",					int(MonsterSpace::eMonsterSoundPanic)),
				value("growling",				int(MonsterSpace::eMonsterSoundGrowling))
			]

			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CScriptSound&,const Fvector &>())
			.def(								constructor<CScriptSound&,const Fvector &,const Fvector &>())
			.def(								constructor<CScriptSound&,const Fvector &,const Fvector &,bool>())

			// monster specific
			.def(								constructor<MonsterSpace::EMonsterSounds>())
			.def(								constructor<MonsterSpace::EMonsterSounds,int>())

			// trader specific
			.def(								constructor<LPCSTR,LPCSTR,MonsterSpace::EMonsterHeadAnimType>())


			.def("set_sound",					(void (CScriptSoundAction::*)(LPCSTR))(CScriptSoundAction::SetSound))
			.def("set_sound",					(void (CScriptSoundAction::*)(const CScriptSound &))(CScriptSoundAction::SetSound))
			.def("set_sound_type",				&CScriptSoundAction::SetSoundType)
			.def("set_bone",					&CScriptSoundAction::SetBone)
			.def("set_position",				&CScriptSoundAction::SetPosition)
			.def("set_angles",					&CScriptSoundAction::SetAngles)
			.def("completed",					(bool (CScriptSoundAction::*)())(CScriptSoundAction::completed)),

		class_<CParticleParams>("particle_params")
			.def(								constructor<>())
			.def(								constructor<const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &,const Fvector &>()),

		class_<CScriptParticleAction>("particle")
			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &, bool>())
			.def(								constructor<LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,const CParticleParams &, bool>())
			.def("set_particle",				&CScriptParticleAction::SetParticle)
			.def("set_bone",					&CScriptParticleAction::SetBone)
			.def("set_position",				&CScriptParticleAction::SetPosition)
			.def("set_angles",					&CScriptParticleAction::SetAngles)
			.def("set_velocity",				&CScriptParticleAction::SetVelocity)
			.def("completed",					(bool (CScriptParticleAction::*)())(CScriptParticleAction::completed)),

		class_<CScriptObjectAction>("object")
			.enum_("state")
			[
				value("idle",					int(MonsterSpace::eObjectActionIdle)),
				value("show",					int(MonsterSpace::eObjectActionShow)),		
				value("hide",					int(MonsterSpace::eObjectActionHide)),		
				value("take",					int(MonsterSpace::eObjectActionTake)),		
				value("drop",					int(MonsterSpace::eObjectActionDrop)),		
				value("strap",					int(MonsterSpace::eObjectActionStrapped)),		
				value("aim1",					int(MonsterSpace::eObjectActionAim1)),		
				value("aim2",					int(MonsterSpace::eObjectActionAim2)),		
				value("reload",					int(MonsterSpace::eObjectActionReload1)),	
				value("reload1",				int(MonsterSpace::eObjectActionReload1)),	
				value("reload2",				int(MonsterSpace::eObjectActionReload2)),	
				value("fire1",					int(MonsterSpace::eObjectActionFire1)),		
				value("fire2",					int(MonsterSpace::eObjectActionFire2)),		
				value("switch1",				int(MonsterSpace::eObjectActionSwitch1)),	
				value("switch2",				int(MonsterSpace::eObjectActionSwitch2)),	
				value("activate",				int(MonsterSpace::eObjectActionActivate)),
				value("deactivate",				int(MonsterSpace::eObjectActionDeactivate)),
				value("use",					int(MonsterSpace::eObjectActionUse)),
				value("turn_on",				int(MonsterSpace::eObjectActionTurnOn)),
				value("turn_off",				int(MonsterSpace::eObjectActionTurnOff)),
				value("dummy",					int(MonsterSpace::eObjectActionDummy))
			]
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*,MonsterSpace::EObjectAction>())
			.def(								constructor<CLuaGameObject*,MonsterSpace::EObjectAction,u32>())
			.def(								constructor<MonsterSpace::EObjectAction>())
			.def(								constructor<LPCSTR,MonsterSpace::EObjectAction>())
			.def("action",						&CScriptObjectAction::SetObjectAction)
			.def("object",						(void (CScriptObjectAction::*)(LPCSTR))(CScriptObjectAction::SetObject))
			.def("object",						(void (CScriptObjectAction::*)(CLuaGameObject*))(CScriptObjectAction::SetObject))
			.def("completed",					(bool (CScriptObjectAction::*)())(CScriptObjectAction::completed)),
			
		class_<CActionCondition>("cond")
			.enum_("cond")
			[
				value("move_end",				int(CActionCondition::MOVEMENT_FLAG	)),
				value("look_end",				int(CActionCondition::WATCH_FLAG)),
				value("anim_end",				int(CActionCondition::ANIMATION_FLAG)),
				value("sound_end",				int(CActionCondition::SOUND_FLAG)),
				value("object_end",				int(CActionCondition::OBJECT_FLAG)),
				value("time_end",				int(CActionCondition::TIME_FLAG)),
				value("act_end",				int(CActionCondition::ACT_FLAG))
			]
			.def(								constructor<>())
			.def(								constructor<u32>())
			.def(								constructor<u32,double>()),

		class_<CEntityAction>("entity_action")
			.def(								constructor<>())
			.def(								constructor<const CEntityAction *>())
			.def("set_action",					(void (CEntityAction::*)(CScriptMovementAction	&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CScriptWatchAction		&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CScriptAnimationAction	&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CScriptSoundAction		&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CScriptParticleAction	&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CScriptObjectAction		&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CActionCondition			&))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(CMonsterAction			&))	(CEntityAction::SetAction))
			.def("move",						&CEntityAction::CheckIfMovementCompleted)
			.def("look",						&CEntityAction::CheckIfWatchCompleted)
			.def("anim",						&CEntityAction::CheckIfAnimationCompleted)
			.def("sound",						&CEntityAction::CheckIfSoundCompleted)
			.def("particle",					&CEntityAction::CheckIfParticleCompleted)
			.def("object",						&CEntityAction::CheckIfObjectCompleted)
			.def("time",						&CEntityAction::CheckIfTimeOver)
			.def("all",							(bool (CEntityAction::*)())(CEntityAction::CheckIfActionCompleted))
			.def("completed",					(bool (CEntityAction::*)())(CEntityAction::CheckIfActionCompleted)),

		class_<CMonsterAction>("act")
			.enum_("type")
			[
				value("rest",					int(MonsterSpace::eGA_Rest)),
				value("eat",					int(MonsterSpace::eGA_Eat)),
				value("attack",					int(MonsterSpace::eGA_Attack)),
				value("panic",					int(MonsterSpace::eGA_Panic))
			]

			.def(								constructor<>())
			.def(								constructor<MonsterSpace::EScriptMonsterGlobalAction>())
			.def(								constructor<MonsterSpace::EScriptMonsterGlobalAction, CLuaGameObject*>())
	];
}

