	////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_space.h
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Dmitriy Iassenev
//	Description : Chimera types and structures
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_DYNAMIC_OBJECTS 			32
#define MAX_DYNAMIC_SOUNDS  			32
#define MAX_HURT_COUNT					32

#define MAX_TIME_RANGE_SEARCH			5000.f

#define R2D(x)							(angle_normalize(x)*180.f/PI)

//#define SILENCE

#ifndef SILENCE
	#define OUT_TEXT(S)						if (g_Alive()) Msg(S);
#else
	#define OUT_TEXT(S)						;
#endif

namespace AI_Chimera {
	// Paths
	enum EPathState {
		ePathStateSearchNode = 0,
		ePathStateBuildNodePath,
		ePathStateBuildTravelLine,
		ePathStateBuilt
	};

	enum EPathType {
		ePathTypeStraight = 0,
		ePathTypeCriteria,
		ePathTypeStraightCriteria,
	};

	// Animations
	enum EPostureAnim {
		ePostureStand = 0,
		ePostureSit,
		ePostureLie
	};

	enum EActionAnim {
		eActionIdle = 0,
		eActionIdleTurnLeft,
		eActionWalkFwd,
		eActionWalkBkwd,
		eActionWalkTurnLeft,
		eActionWalkTurnRight,
		eActionRun,
		eActionRunTurnLeft,
		eActionRunTurnRight,
		eActionAttack,
		eActionAttack2,
		eActionAttackTurnLeft,
		eActionEat,
		eActionDamage,
		eActionScared,
		eActionDie,
		eActionLieDown,
		eActionStandUp,
		eActionCheckCorpse,
	};

};


