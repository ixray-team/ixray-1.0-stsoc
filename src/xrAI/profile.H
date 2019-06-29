//////////////////////////////////////////////////////////
//	Module			:	Time routines using RDTSC 		//
//	File			:	_time.h							//
//	Creation date	:	31.08.2002						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#pragma once

typedef double	f64;
typedef __int64 s64;

//#define __PROFILE__

#ifdef __PROFILE__
	#define MAX_TIMER_COUNT		  64

	extern u64	qwaTimerCalls[MAX_TIMER_COUNT];
	extern u64	qwaTimerTimes[MAX_TIMER_COUNT];
	extern u32	dwTimerCount;
	extern u64	timer_overhead;

	#define TIMER_EXTERN_DECLARE(a) \
		extern u64 qwTemp1##a, qwTemp2##a, &qwTime##a, &qwCount##a;

	#define TIMER_DECLARE(a) \
		u64 qwTemp1##a, qwTemp2##a, &qwTime##a = qwaTimerTimes[dwTimerCount] = 0, &qwCount##a = qwaTimerCalls[dwTimerCount++] = 0;
	#define TIMER_INIT(a) \
		qwTime##a = qwCount##a = 0;

	#define TIMER_START(a) \
		qwTemp1##a = CPU::GetCycleCount();

	#define TIMER_STOP(a) \
		qwTemp2##a = CPU::GetCycleCount();\
		qwTime##a += qwTemp2##a - qwTemp1##a - CPU::cycles_overhead;\
		qwCount##a++;

	#define TIMER_STOP_PREVIOUS(a,b) \
		qwTemp2##a = CPU::GetCycleCount();\
		qwTime##a += qwTemp2##a - qwTemp1##b - CPU::cycles_overhead;\
		qwCount##a++;

	#define PRINT_TIMER_HEADER {\
		Msg("+----------+-----------+-----------------------------------------------------+");\
		Msg("|   time   |   count   |                        function                     |");\
		Msg("+----------+-----------+-----------------------------------------------------+");\
	}

	#define TIMER_PRINT(a) \
		Msg("|%10.5f|%11I64u|%52s |",f64(s64(qwTime##a))/f64(s64(CPU::cycles_per_second)),qwCount##a,#a);

	#define PRINT_PROFILE_TIME {\
		u64 qwTemp1 = 0, qwTemp2 = 0;\
		for (u32 _i_=0; _i_ < dwTimerCount; _i_++) {\
			qwTemp1 += qwaTimerCalls[_i_];\
			qwTemp2 += qwaTimerTimes[_i_];\
		}\
		Msg("+----------+-----------+-----------------------------------------------------+");\
		Msg("|%10.5f|%11I64u|                                               Total |",f64(s64(qwTemp2))/f64(s64(CPU::cycles_per_second)),qwTemp1);\
		Msg("+==========+===========+=====================================================+");\
		Msg("|%10.5f|%11I64u|                                           Profiling |",f64(s64(qwTemp1*timer_overhead))/f64(s64(CPU::cycles_per_second)),qwTemp1);\
		Msg("+----------+-----------+-----------------------------------------------------+");\
	}

	#define PRINT_TIMERS {\
		PRINT_TIMER_HEADER\
		TIMER_PRINT(AdjustPoint)\
		TIMER_PRINT(AssignAngle)\
		TIMER_PRINT(ComputeCircles)\
		TIMER_PRINT(CrossProduct2D)\
		TIMER_PRINT(ComputeTangent)\
		TIMER_PRINT(BuildCircleTrajectory)\
		TIMER_PRINT(BuildLineTrajectory)\
		TIMER_PRINT(BuildTrajectory1)\
		TIMER_PRINT(BuildTrajectory2)\
		TIMER_PRINT(ComputeTrajectory)\
		TIMER_PRINT(ComputePath)\
		TIMER_PRINT(BuildDetailPath)\
		TIMER_PRINT(ValidVertexID)\
		TIMER_PRINT(VertexPosition)\
		TIMER_PRINT(Inside)\
		TIMER_PRINT(VertexPlaneY)\
		TIMER_PRINT(CheckVertexInDirection)\
		TIMER_PRINT(CheckPositionInDirection)\
		TIMER_PRINT(CreateStraightPath)\
		TIMER_PRINT(BCT_CPID)\
		TIMER_PRINT(BCT_AP)\
		PRINT_PROFILE_TIME\
	}
	#define RESET_ALL_TIMERS {\
		Memory.mem_fill(qwaTimerCalls,0,sizeof(qwaTimerCalls));\
		Memory.mem_fill(qwaTimerTimes,0,sizeof(qwaTimerTimes));\
	}

#else

	#define TIMER_EXTERN_DECLARE(a)
	#define TIMER_DECLARE(a)
	#define TIMER_INIT(a)
	#define TIMER_START(a)
	#define TIMER_STOP(a)
	#define TIMER_STOP_PREVIOUS(a,b)
	#define TIMER_PRINT(a)
	#define PRINT_PROFILE_TIME
	#define PRINT_TIMER_HEADER
	#define PRINT_TIMERS
	#define RESET_ALL_TIMERS

#endif

TIMER_EXTERN_DECLARE(TimerOverhead)
TIMER_EXTERN_DECLARE(AdjustPoint)
TIMER_EXTERN_DECLARE(AssignAngle)
TIMER_EXTERN_DECLARE(ComputeCircles)
TIMER_EXTERN_DECLARE(CrossProduct2D)
TIMER_EXTERN_DECLARE(ComputeTangent)
TIMER_EXTERN_DECLARE(BuildCircleTrajectory)
TIMER_EXTERN_DECLARE(BuildLineTrajectory)
TIMER_EXTERN_DECLARE(BuildTrajectory1)
TIMER_EXTERN_DECLARE(BuildTrajectory2)
TIMER_EXTERN_DECLARE(ComputeTrajectory)
TIMER_EXTERN_DECLARE(ComputePath)
TIMER_EXTERN_DECLARE(BuildDetailPath)
TIMER_EXTERN_DECLARE(ValidVertexID)
TIMER_EXTERN_DECLARE(VertexPosition)
TIMER_EXTERN_DECLARE(Inside)
TIMER_EXTERN_DECLARE(VertexPlaneY)
TIMER_EXTERN_DECLARE(CheckVertexInDirection)
TIMER_EXTERN_DECLARE(CheckPositionInDirection)
TIMER_EXTERN_DECLARE(CreateStraightPath)
TIMER_EXTERN_DECLARE(BCT_CPID)
TIMER_EXTERN_DECLARE(BCT_AP)

#ifdef __PROFILE__
	IC void init_timers()
	{
		SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

		timer_overhead = 0;
		for (s32 i=0; i<1048576; i++) {
			u64 qwStartTime = CPU::GetCycleCount();
			TIMER_START(TimerOverhead);
			TIMER_STOP(TimerOverhead);
			u64 qwStopTime = CPU::GetCycleCount();;
			timer_overhead += qwStopTime - qwStartTime - CPU::cycles_overhead;
		}
		timer_overhead >>= 20;
		TIMER_INIT(TimerOverhead);

		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	}
#else
	IC void init_timers()
	{
	}
#endif