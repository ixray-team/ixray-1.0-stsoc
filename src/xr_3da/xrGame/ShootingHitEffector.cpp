#include "stdafx.h"
/*
#include "ShootingHitEffector.h"

#define SHOOTING_EFFECTOR_TYPE_ID 7
CShootingHitEffectorPP::CShootingHitEffectorPP(SShootingEffector* eff)//const SPPInfo &ppi, float life_time, float attack_time, float release_time) 
	: CEffectorPP(EEffectorPPType(SHOOTING_EFFECTOR_TYPE_ID), eff->time)
{
	se = eff;
	VERIFY(!fsimilar(se->time_release, 1.0f));
	VERIFY(!fis_zero(se->time_attack));
}

BOOL CShootingHitEffectorPP::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	// amount of time passed in percents
	float time_past_perc = (se->time - fLifeTime) / se->time;

	float factor;
	if (time_past_perc < se->time_attack) 
		factor = time_past_perc / se->time_attack;
	else if ((time_past_perc >= se->time_attack) && (time_past_perc <= se->time_attack)) 
		factor = 1.0f;
	else 
		factor = (1.0f - time_past_perc) / (1.0f - se->time_attack);

	clamp		(factor,0.01f,1.0f);

	pp.lerp		(pp_identity, se->ppi, factor);

	return TRUE;
}
*/