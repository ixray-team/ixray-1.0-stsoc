////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for chimera class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"
#include "..\\rat\\ai_rat.h"
#include "..\\..\\actor.h"

using namespace AI_Chimera;

void CAI_Chimera::Think()
{
	if (!g_Alive()) return;

	//return;

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	vfUpdateParameters		();

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),A,B,C,D,E,F,H,I,J,K);
#endif

	// A - я слышу опасный звук
	// B - я слышу неопасный звук
	// С - я вижу очень опасного врага
	// D - я вижу опасного врага
	// E - я вижу равного врага
	// F - я вижу слабого врага
	// H - враг выгодный
	// I - враг видит меня
	// J - A | B
	// K - C | D | E | F 

	VisionElem ve;

	if (Motion.m_tSeq.Active())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		//- FSM 1-level 

		//if (flagEnemyLostSight && H && (E || F) && !A) SetState(stateFindEnemy);	// поиск врага
		if (C && H && I)		SetState(statePanic);
		else if (C && H && !I)		SetState(statePanic);
		else if (C && !H && I)		SetState(statePanic);
		else if (C && !H && !I) 	SetState(statePanic);
		else if (D && H && I)		SetState(stateAttack);
		else if (D && H && !I)		SetState(stateAttack);  //тихо подобраться и начать аттаку
		else if (D && !H && I)		SetState(statePanic);
		else if (D && !H && !I) 	SetState(stateHide);	// отход перебежками через укрытия
		else if (E && H && I)		SetState(stateAttack); 
		else if (E && H && !I)  	SetState(stateAttack);  //тихо подобраться и начать аттаку
		else if (E && !H && I) 		SetState(stateDetour); 
		else if (E && !H && !I)		SetState(stateDetour); 
		else if (F && H && I) 		SetState(stateAttack); 		
		else if (F && H && !I)  	SetState(stateAttack); 
		else if (F && !H && I)  	SetState(stateDetour); 
		else if (F && !H && !I) 	SetState(stateHide);
		else if (A && !K && !H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
		else if (A && !K && H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
		else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
		else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
		else if (GetCorpse(ve) && ve.obj->m_fFood > 1)	
			SetState(stateEat);
		else						SetState(stateRest); 

		//---

		CurrentState->Execute(m_dwCurrentUpdate);

		// проверяем на завершённость
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);

	ControlAnimation();
}