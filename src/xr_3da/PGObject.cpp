//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PGObject.h"
#include "..\ParticleCustom.h"
#include "..\render.h"

CPGObject::CPGObject	(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove			= bAutoRemove;

	// create visual
	renderable.visual		= Render->model_CreateParticles(p_name);
	VERIFY					(renderable.visual);
	IParticleCustom* V		= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	
	float time_limit		= V->GetTimeLimit();
	if (bAutoRemove&&(time_limit>=0.f)){
		m_iLifeTime			= iFloor(time_limit*1000.f);
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
//.	PS::CParticleEffect* V	= dynamic_cast<PS::CParticleEffect*>(renderable.visual); R_ASSERT(V);
}

LPCSTR CPGObject::dbg_ref_name()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	return V->Name();
	
}
//----------------------------------------------------
void CPGObject::Play()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	V->Play			();
}

void CPGObject::Stop()
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	V->Stop			();
}

void CPGObject::play_at_pos(const Fvector& pos)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	Fmatrix m; m.identity(); m.c.set(pos); 
	Fvector vel={0,0,0};
	V->UpdateParent	(m,vel);
	V->Play			();
}

void CPGObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);

	// visual
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	V->OnFrame			(dt);

	// spatial	(+ workaround occasional bug inside particle-system)
	if (_valid(renderable.visual->vis.sphere))
	{
		spatial.center		= renderable.visual->vis.sphere.P;
		spatial.radius		= renderable.visual->vis.sphere.R;
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
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	V->UpdateParent		(m,zero_vel);
}

void CPGObject::UpdateParent		(const Fmatrix& m, const Fvector& vel)
{
	IParticleCustom* V	= dynamic_cast<IParticleCustom*>(renderable.visual); R_ASSERT(V);
	V->UpdateParent		(m,vel);
}

Fvector& CPGObject::Position		()
{
	return renderable.visual->vis.sphere.P;
}

void CPGObject::renderable_Render	()
{
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);
}
