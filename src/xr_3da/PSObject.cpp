//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSObject.h"
#include "../render.h"

CPSObject::CPSObject	(LPCSTR ps_name, IRender_Sector* S, BOOL bAutoRemove)
{
	m_bAutoRemove		= bAutoRemove;

	// create visual
	/*
	renderable.visual	= Render->model_CreatePS	(ps_name,&m_Emitter);
	VERIFY				(renderable.visual);
	*/

	// spatial
	spatial.type			= 0;
	spatial.sector			= S;

	// sheduled
	shedule.t_min			= 20;
	shedule.t_max			= 50;
	shedule_register		();
}

//----------------------------------------------------
CPSObject::~CPSObject	()
{
	shedule_unregister		();
}

void CPSObject::shedule_Update		(u32 dt)
{
	inherited::shedule_Update	(dt);

	return;

	// visual
	/*
	CPSVisual* V	= (CPSVisual*)	renderable.visual;
	V->Update		(dt);
	if (m_Emitter.m_Flags.is(PS_EM_PLAY_ONCE))
	{
		if ((0==V->ParticleCount())&&!m_Emitter.IsPlaying()) Stop();
	}

	// spatial
	spatial.center		= V->vis.sphere.P;
	spatial.radius		= V->vis.sphere.R;
	if (0==spatial.type)	{
		spatial.type		= 0; // STYPE_RENDERABLE;
		spatial_register	();
	} else {
		spatial_move		();
	}
	*/
}

void CPSObject::play_at_pos			(const Fvector& pos)
{
	/*
	m_Emitter.m_Position.set(pos);
	m_Emitter.Play			();
	*/
}

void CPSObject::Stop				()
{
	m_iLifeTime				= -1;
	//. ((CPSVisual*)renderable.visual)->Stop();
}

void CPSObject::renderable_Render	()
{
	Debug.fatal("Unreachable function reached 'CPSObject::renderable_Render'");
	/*
	::Render->set_Transform			(&Fidentity);
	::Render->add_Visual			(renderable.visual);
	*/
}
