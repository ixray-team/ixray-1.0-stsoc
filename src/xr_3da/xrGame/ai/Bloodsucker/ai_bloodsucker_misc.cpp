#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_misc.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// Реализация невидимости (мерцания)
///////////////////////////////////////////////////////////////////////////////////////////////

CBloodsuckerVisibility::CBloodsuckerVisibility()
{
	timeStartBlinking		= 0;
	timeLastBlink			= 0; 
	bCurVisibility			= flagVisibility	= true;

	timeStartRestore		= 0;
	timeStartInvisible		= 0;

	bBlinking				= false;
}

void CBloodsuckerVisibility::Load(LPCTSTR section)
{
	timeBlinking		= pSettings->r_u32(section,"BlinkTime");
	timeBlinkInterval	= pSettings->r_u32(section,"BlinkMicroInterval");
	timeRestoreInterval	= pSettings->r_u32(section,"RestoreInterval");
	timeInvisibilityMin	= pSettings->r_u32(section,"InvisibilityTimeMin");;
	timeInvisibilityMax	= pSettings->r_u32(section,"InvisibilityTimeMax");;
}

bool CBloodsuckerVisibility::Switch (bool bVis) 
{
	if ((bVis == false) && !IsInvisibilityReady()) return false;		// если ещё не восстановился
	if (bBlinking) return false;										// если в переходном состоянии
	if (bVis == flagVisibility) return false;							// если уже находимся в этом состоянии

	timeStartBlinking			= timeCurrent;
	bCurVisibility				= flagVisibility;
	flagVisibility				= bVis;
	timeLastBlink				= 0; 
	timeBlinkIntervalCurrent	= timeBlinkInterval;

	bBlinking					= true;
	
	if (flagVisibility) timeStartRestore = timeCurrent; // если монстр стал видим
	else {
		timeStartInvisible = timeCurrent;
		timeInvisibleInterval = ::Random.randI(timeInvisibilityMin,timeInvisibilityMax);
	}
	
	return true;
}	

bool CBloodsuckerVisibility::Update () 
{
	timeCurrent = Level().timeServer();

	if (timeStartBlinking + timeBlinking > timeCurrent) {
		if (timeLastBlink + timeBlinkInterval < timeCurrent) {
			// blink
			timeLastBlink = timeCurrent; 
			bCurVisibility = !bCurVisibility;

			timeBlinkIntervalCurrent = (TTime) (timeBlinkInterval - (float)(((timeBlinkInterval - 1) * (float)(timeCurrent - timeStartBlinking)) / timeBlinking));
		}
		return bCurVisibility;
	} else {
		bCurVisibility = flagVisibility;
		bBlinking = false;
	}

	// если время невидимости вышло
	if (!flagVisibility && (timeStartInvisible + timeInvisibleInterval < timeCurrent)) Switch(true);

	return flagVisibility;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Другие функции
///////////////////////////////////////////////////////////////////////////////////////////////

//void CAI_Bloodsucker::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
//{
//	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
//	index3 = -1;
//
//	switch(motion) {
//		case eAnimStandIdle:		index1 = 0; index2 = 0;	 break;
//		case eAnimStandTurnLeft:	index1 = 0; index2 = 2;  break;
//		case eAnimStandTurnRight:	index1 = 0; index2 = 1;  break;
//		
//		case eAnimWalkFwd:			index1 = 0; index2 = 3;	 break;
//		case eAnimWalkBkwd:			index1 = 0; index2 = 4;  break;
//		case eAnimRun:				index1 = 0; index2 = 7;  break;
//
//		case eAnimAttack:			index1 = 0; index2 = 10; break;
//		
//		case eAnimEat:				index1 = 1; index2 = 14; break;
//		case eAnimDie:				index1 = 0; index2 = 0;	 break;
//		
//		case eAnimSitStandUp:		index1 = 1; index2 = 21; break;
//
//		case eAnimSitIdle:			index1 = 1; index2 = 0;	 break; 
//		case eAnimStandSitDown:		index1 = 0; index2 = 18; break; 
//		case eAnimSleep:			index1 = 2; index2 = 24; break;
//		case eAnimSitToSleep:		index1 = 1; index2 = 23; break;
//
//		//default:					NODEFAULT;
//	} 
//
////	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());
//
//}

//void CAI_Bloodsucker::CheckTransitionAnims()
//{
//	if (((m_tAnimPrevFrame == eAnimSitIdle) && (m_tAnim != eAnimSitIdle)) ||
//		((m_tAnimPrevFrame == eAnimSleep) && (m_tAnim != eAnimSleep))){
////		Motion.m_tSeq.Add(eAnimSitStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED,STOP_ANIM_END);
////		Motion.m_tSeq.Switch();
//	}
//}
