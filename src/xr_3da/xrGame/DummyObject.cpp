// DummyObject.cpp: implementation of the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DummyObject.h"
#include "..\ObjectAnimator.h"
#include "..\psvisual.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyObject::CDummyObject	()
{
	style					= 0;
	s_animator				= NULL;
	s_particles				= NULL;
	s_sound.feedback		= NULL;
	m_pPhysicsShell			= 0;
}

CDummyObject::~CDummyObject	()
{
	::Sound->destroy			(s_sound);
	::Render->model_Delete	(s_particles);
	xr_delete				(s_animator);
	xr_delete				(m_pPhysicsShell);
}

void CDummyObject::Load		(LPCSTR section)
{
	inherited::Load			(section);

	/*
	if (pSettings->line_exist(section,"motions")){
		style			|= esAnimated;
	}
	if (Visual()->Type==MT_SKELETON){
		style			|= esSkeleton;
		PKinematics(Visual())->PlayCycle	("idle");
	}
	if (pSettings->line_exist(section,"sound"))
	{
		LPCSTR N = pSettings->r_string(section,"sound");
		::Sound->create(sndDummy,TRUE,N);
	}

	Sector_Detect();
	*/
}

BOOL CDummyObject::net_Spawn(LPVOID DC)
{
	inherited::net_Spawn	(DC);
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_Dummy				*E	= dynamic_cast<CSE_Dummy*>(e);

	// 
	setVisible				(TRUE);

	relation.set			(XFORM());

	style					= E->s_style;
	if (style&esAnimated)		{
		// Load animator
		s_animator				= xr_new<CObjectAnimator>	();
		s_animator->Load		(E->s_Animation);
		s_animator->PlayMotion	("idle",true);
	}
	if (style&esModel)			{
		// Load model
		cNameVisual_set			(E->s_Model);
	}
	if (style&esParticles)		{
		// Load model
		s_particles				= ::Render->model_CreatePS	(E->s_Particles,&s_emitter);
		CPSVisual* V			= dynamic_cast<CPSVisual*>	(s_particles);
		if (V)					{
			s_emitter.m_Position.set	(Position());
			s_emitter.Play				();
		}
	}
	if (style&esSound)			{
		// Load ref_sound
		::Sound->create			(s_sound,TRUE,E->s_Sound);
		::Sound->play_at_pos		(s_sound,0,Position(),true);
	}

	CKinematics* V			= PKinematics	(Visual());
	if (V)					V->PlayCycle	("idle");

	if (!s_animator && (style&esModel) && (0==m_pPhysicsShell))
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		E->add_Box							(obb);

		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(200.f);
		m_pPhysicsShell->Activate			(XFORM(),0,XFORM(),true);
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
	}

	return TRUE;
}

void CDummyObject::shedule_Update		(u32 dt)
{
	inherited::shedule_Update	(dt);

	if (s_particles)	dynamic_cast<CPSVisual*>(s_particles)->Update(dt);
}

void CDummyObject::UpdateCL		()
{
	if (s_animator)
	{
		s_animator->OnMove		();
		XFORM().set				(s_animator->GetRotate());
		Position().set			(s_animator->GetPosition());
		if (style&esRelativePosition){
			R_ASSERT2(0,"CDummyObject: Relative position error.");
			XFORM().mulB_43	(relation);
		}
	} else {
		if (m_pPhysicsShell)		
		{
			m_pPhysicsShell->Update	();
			XFORM().set			(m_pPhysicsShell->mXFORM);
			Position().set			(XFORM().c);
		}
	}
	XFORM().set				(XFORM());

	if (s_particles)			
	{
		s_emitter.m_Position.set				(Position());
	}
	if (s_sound.feedback)
	{
		s_sound.set_position	(Position());
		s_sound.set_range		(10,1000);
	}
}

void CDummyObject::renderable_Render	()
{
	inherited::renderable_Render();
	if (s_particles)	::Render->add_Visual(s_particles);
}
