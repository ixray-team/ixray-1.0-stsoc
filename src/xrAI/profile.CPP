//////////////////////////////////////////////////////////
//	Module			:	Time routines using RDTSC 		//
//	File			:	_time.cpp						//
//	Creation date	:	31.08.2002						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"
#include "profile.h"

#ifdef __PROFILE__
	u64	qwaTimerCalls	[MAX_TIMER_COUNT];
	u64	qwaTimerTimes	[MAX_TIMER_COUNT];
	u32	dwTimerCount	= 0;
	u64	timer_overhead	= 0;
#endif

TIMER_DECLARE(TimerOverhead)
TIMER_DECLARE(AdjustPoint)
TIMER_DECLARE(AssignAngle)
TIMER_DECLARE(ComputeCircles)
TIMER_DECLARE(CrossProduct2D)
TIMER_DECLARE(ComputeTangent)
TIMER_DECLARE(BuildCircleTrajectory)
TIMER_DECLARE(BuildLineTrajectory)
TIMER_DECLARE(BuildTrajectory1)
TIMER_DECLARE(BuildTrajectory2)
TIMER_DECLARE(ComputeTrajectory)
TIMER_DECLARE(ComputePath)
TIMER_DECLARE(BuildDetailPath)
TIMER_DECLARE(ValidVertexID)
TIMER_DECLARE(VertexPosition)
TIMER_DECLARE(Inside)
TIMER_DECLARE(VertexPlaneY)
TIMER_DECLARE(CheckVertexInDirection)
TIMER_DECLARE(CheckPositionInDirection)
TIMER_DECLARE(CreateStraightPath)
TIMER_DECLARE(BCT_CPID)
TIMER_DECLARE(BCT_AP)
