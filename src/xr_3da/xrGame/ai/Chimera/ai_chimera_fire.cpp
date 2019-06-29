////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_fire.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Fire and enemy parameters for all the chimera monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"
#include "..\\..\\actor.h"

float CAI_Chimera::EnemyHeuristics(CEntity* E)
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


void CAI_Chimera::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
	SHurt	tHurt;
	tHurt.dwTime	= Level().timeServer();
	if (0 != (tHurt.tpEntity = dynamic_cast<CEntity*>(who)))
		vfUpdateHurt(tHurt);

	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);

	if (g_Health() - amount <= 0) {
	}
}

int	 CAI_Chimera::ifFindHurtIndex(CEntity *tpEntity)
{
	for (int i=0; i<(int)m_tpaHurts.size(); i++)
		if (m_tpaHurts[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

void CAI_Chimera::vfAddHurt(const SHurt &tHurt)
{
	if (m_tpaHurts.size() >= MAX_HURT_COUNT)
		m_tpaHurts.erase(m_tpaHurts.begin());
	m_tpaHurts.push_back(tHurt);
}

void CAI_Chimera::vfUpdateHurt(const SHurt &tHurt)
{
	int iIndex = ifFindHurtIndex(tHurt.tpEntity);
	if (iIndex != -1)
		m_tpaHurts[iIndex].dwTime = tHurt.dwTime;
	else
		vfAddHurt(tHurt);
}

float CAI_Chimera::CorpHeuristics(CEntity* E)
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


void CAI_Chimera::FillAttackStructure(u32 i, TTime t)
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
			m_tAttack.dist		= 3.f;
			
			Center(m_tAttack.TraceFrom);
			break;
		case 1:
			m_tAttack.time_from = 500;
			m_tAttack.time_to	= 600;
			m_tAttack.dist		= 2.5f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 2:
			m_tAttack.time_from = 600;
			m_tAttack.time_to	= 700;
			m_tAttack.dist		= 3.5f;
			Center(m_tAttack.TraceFrom);
			break;
		case 3:
			m_tAttack.time_from = 800;
			m_tAttack.time_to	= 900;
			m_tAttack.dist		= 1.0f;
			
			Center(m_tAttack.TraceFrom);
			tempV.set(-0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 4:
			m_tAttack.time_started = 0;
			break;
		case 5:
			m_tAttack.time_from = 1500;
			m_tAttack.time_to	= 1600;
			m_tAttack.dist		= 3.0f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 6:
			m_tAttack.time_started = 0;
			break;
	}
}