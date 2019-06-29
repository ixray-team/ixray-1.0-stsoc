//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PGObject.h"
#include "..\ParticleEffect.h"
#include "..\render.h"

CPGObject::CPGObject	(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove			= bAutoRemove;

	// create visual
	renderable.visual		= Render->model_CreatePE(ps_name);
	VERIFY					(renderable.visual);
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	
	if (bAutoRemove&&V->GetDefinition()->m_Flags.is(PS::CPEDef::dfTimeLimit)){
		m_iLifeTime	= V->GetDefinition()->m_TimeLimit;
	}else{
		R_ASSERT3	(!m_bAutoRemove,"Can't set auto-remove flag for looped particle system.",ps_name);
	}

	// spatial
	spatial.type			= 0;
	spatial.sector			= S;

	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
	shedule_register		();
}

//----------------------------------------------------
CPGObject::~CPGObject()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
}

LPCSTR CPGObject::dbg_ref_name()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	return V->GetDefinition()->m_Name;
	
}
//----------------------------------------------------
void CPGObject::Play()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->Play			();
}

void CPGObject::Stop()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->Stop			();
}

void CPGObject::play_at_pos(const Fvector& pos)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel);
	V->Play			();
}

void CPGObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);

	// visual
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
	V->OnFrame			(dt);

	// spatial	(+ workaround occasional bug inside particle-system)
	if (_valid(V->vis.sphere))
	{
		spatial.center		= V->vis.sphere.P;
		spatial.radius		= V->vis.sphere.R;
		if (0==spatial.type)	{
			// First 'valid' update - register
			spatial.type						= STYPE_RENDERABLE;
			spatial_register					();
		} else {
			spatial_move		();
		}
	}

	// Msg	("update(%s): %3.1f,%3.1f,%3.1f,%3.1f",V->GetDefinition()->m_Name,VPUSH(spatial.center),spatial.radius);
}

static const Fvector zero_vel		= {0.f,0.f,0.f};
void CPGObject::SetTransform		(const Fmatrix& m)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,zero_vel);
}

void CPGObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	V->UpdateParent		(m,vel);
}

Fvector& CPGObject::Position		()
{
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	return V->vis.sphere.P;
}

void CPGObject::renderable_Render	()
{
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);

	/*
	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual);	R_ASSERT(V);
	Msg						("render(%s): %3.1f,%3.1f,%3.1f,%3.1f",V->GetDefinition()->m_Name,VPUSH(spatial.center),spatial.radius);
	*/
}
