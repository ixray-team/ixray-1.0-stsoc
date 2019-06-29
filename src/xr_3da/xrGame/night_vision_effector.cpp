#include "stdafx.h"
/*
#include "night_vision_effector.h"

#include "actor.h"
#include "../CameraManager.h"



//////////////////////////////////////////////////////////////////////////
// NightVision
//////////////////////////////////////////////////////////////////////////
CNightVisionEffectorPP::CNightVisionEffectorPP(const SPPInfo &ppi, float attack_time, float release_time) :
						CEffectorPP(EEffectorPPType(NIGHT_VISION_EFFECTOR_TYPE_ID), flt_max)
{
	state		= ppi;
	
	m_attack	= ((fis_zero(attack_time)) ? 0.5f : attack_time);
	m_release	= ((fis_zero(release_time)) ? 0.5f : release_time);

	VERIFY(!fsimilar(m_release, 1.0f));
	VERIFY(!fis_zero(m_attack));

	m_total = m_attack + m_release;
	fLifeTime = m_attack + m_release;

	m_status = FADE_IN;
}


BOOL CNightVisionEffectorPP::Process(SPPInfo& pp)
{
	float old_life_time = fLifeTime;
	inherited::Process(pp);

	if(LOOP == m_status)
		fLifeTime = old_life_time;

	// amount of time passed in percents
	float time_past = m_total - fLifeTime;

	float factor;
	if (time_past < m_attack)
		factor = time_past / m_attack;
	else
	{
		switch(m_status)
		{
		case FADE_OUT:
			factor = 1.f - (time_past - m_attack)/m_release;
			break;
		case FADE_IN:
			m_status = LOOP;
		case LOOP:
			factor = 1.f;
			break;
		}
	}
	clamp(factor,0.01f,1.0f);

	pp.lerp	(pp_identity, state, factor);

	return TRUE;
}


CNightVisionEffector::CNightVisionEffector()
{
	p_effector = NULL;
}



void CNightVisionEffector::Load(LPCSTR ppi_section)
{
	SPPInfo& ppi =  state;

	ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	ppi.gray			= pSettings->r_float(ppi_section,"gray");
	ppi.blur			= pSettings->r_float(ppi_section,"blur");
	ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &ppi.color_base.r, &ppi.color_base.g, &ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &ppi.color_gray.r, &ppi.color_gray.g, &ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &ppi.color_add.r,  &ppi.color_add.g,  &ppi.color_add.b);

	m_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_release	= pSettings->r_float(ppi_section,"time_release");
}


bool CNightVisionEffector::Start	()
{
	VERIFY(p_effector == NULL);

	if(Actor()->Cameras().GetPPEffector(EEffectorPPType(NIGHT_VISION_EFFECTOR_TYPE_ID))) 
		return false;

	p_effector = xr_new<CNightVisionEffectorPP>(state, m_attack, m_release);
	Actor()->Cameras().AddPPEffector(p_effector);

	return true;
}


bool CNightVisionEffector::Stop	()
{
	if(!p_effector) return false;
	p_effector->m_status = CNightVisionEffectorPP::FADE_OUT;
	p_effector = NULL;

	return true;
}

bool CNightVisionEffector::IsActive	()
{
	if(Actor()->Cameras().GetPPEffector(EEffectorPPType(NIGHT_VISION_EFFECTOR_TYPE_ID)))
		return true;
	else
		return false;
}
*/