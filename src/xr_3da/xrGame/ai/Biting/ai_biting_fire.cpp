////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fire.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Fire and enemy parameters for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"

float CAI_Biting::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team

	if (!E->g_Alive())
		return flt_max;		// don't attack dead enemies

	float	g_strength = E->g_Armor()+E->g_Health();

	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strength);
	return  f1*f2;
}


void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	vfNormalizeSafe(m_tHitDir);
	m_tHitPosition = who->Position();
	
	SHurt	tHurt;
	tHurt.dwTime	= Level().timeServer();
	if (0 != (tHurt.tpEntity = dynamic_cast<CEntity*>(who)))
		vfUpdateHurt(tHurt);

	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);


	// Play hit-ref_sound
	ref_sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,Position());
	}
	
	if (g_Health() - amount <= 0) {
	}
}

int	 CAI_Biting::ifFindHurtIndex(CEntity *tpEntity)
{
	for (int i=0; i<(int)m_tpaHurts.size(); i++)
		if (m_tpaHurts[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

void CAI_Biting::vfAddHurt(const SHurt &tHurt)
{
	if (m_tpaHurts.size() >= MAX_HURT_COUNT)
		m_tpaHurts.erase(m_tpaHurts.begin());
	m_tpaHurts.push_back(tHurt);
}

void CAI_Biting::vfUpdateHurt(const SHurt &tHurt)
{
	int iIndex = ifFindHurtIndex(tHurt.tpEntity);
	if (iIndex != -1)
		m_tpaHurts[iIndex].dwTime = tHurt.dwTime;
	else
		vfAddHurt(tHurt);
}

float CAI_Biting::CorpHeuristics(CEntity* E)
{
	if (!E->g_Alive()) {
		CEntityAlive *tpEntityAlive = dynamic_cast<CEntityAlive *>(E);
		if ( tpEntityAlive && (m_dwCurrentUpdate - tpEntityAlive->m_dwDeathTime < m_dwEatCorpseInterval) && 
			(tpEntityAlive->m_fFood > 0) && (m_bEatMemberCorpses || (E->g_Team() != g_Team())) && 
			(m_bCannibalism || (E->SUB_CLS_ID != SUB_CLS_ID)))

			return (tpEntityAlive->m_fFood*tpEntityAlive->m_fFood)*Position().distance_to(E->Position());
		else
			return flt_max;
	}
	else
		return flt_max;
}


void CAI_Biting::FillAttackStructure(u32 i, TTime t)
{
	
	m_tAttack.i_anim		= i;
	m_tAttack.time_started	= t;
	m_tAttack.b_fire_anyway = false;
	m_tAttack.b_attack_rat	= false;

	Fvector tempV;

	switch (m_tAttack.i_anim) {
		case 0:
			m_tAttack.time_from = 700;
			m_tAttack.time_to	= 800;
			m_tAttack.dist		= 2.f;
			Center(m_tAttack.TraceFrom);
			break;
		case 1:
			m_tAttack.time_from = 600;
			m_tAttack.time_to	= 800;
			m_tAttack.dist		= 2.5f;
			Center(m_tAttack.TraceFrom);
			break;
		case 2:
			m_tAttack.time_from = 1100;
			m_tAttack.time_to	= 1250;
			m_tAttack.dist		= 1.5f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.1f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 3:
			m_tAttack.time_from = 1300;
			m_tAttack.time_to	= 1400;
			m_tAttack.dist		= 0.6f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.1f,0.f,0.f);
			m_tAttack.TraceFrom.sub(tempV);
			break;
		case 4:
			m_tAttack.time_from = 600;
			m_tAttack.time_to	= 800;
			m_tAttack.dist		= 2.6f;
			Center(m_tAttack.TraceFrom);
			tempV.set(-0.5f,0.f,0.5f);
			m_tAttack.TraceFrom.add(tempV);
			m_tAttack.b_attack_rat = true;
			break;
		case 5:
			m_tAttack.time_from = 700;
			m_tAttack.time_to	= 850;
			m_tAttack.b_fire_anyway = true;
			break;
	}
}

