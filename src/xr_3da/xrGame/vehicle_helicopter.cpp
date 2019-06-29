////////////////////////////////////////////////////////////////////////////
//	Module 		: vehicle_helicopter.cpp
//	Created 	: 13.02.2004
//  Modified 	: 13.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Vehicle helicopter class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vehicle_helicopter.h"
#include "../objectanimator.h"
#include "xrserver_objects_alife.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "actor.h"
#include "../skeletonanimated.h"

CVehicleHelicopter::CVehicleHelicopter		()
{
	init				();
}

CVehicleHelicopter::~CVehicleHelicopter		()
{
	xr_delete			(m_animator);
}

void CVehicleHelicopter::init				()
{
	m_animator			= xr_new<CObjectAnimator>();
}

void CVehicleHelicopter::Load				(LPCSTR section)
{
	inherited::Load		(section);
}

void CVehicleHelicopter::reinit				()
{
	inherited::reinit	();
}

void CVehicleHelicopter::reload				(LPCSTR section)
{
	inherited::reload	(section);
}

BOOL CVehicleHelicopter::net_Spawn			(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	// assigning m_animator here
	CSE_Abstract		*abstract=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli	= smart_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT			(Visual()&&smart_cast<CKinematics*>(Visual()));
	CSkeletonAnimated	*A= smart_cast<CSkeletonAnimated*>(Visual());
	if (A) {
		A->PlayCycle		(*heli->startup_animation);
		A->CalculateBones	();
	}
//	m_animator->Load	(heli->get_motion());
//	m_animator->Play	(true);

	m_engine_sound.create(TRUE,*heli->engine_sound);
	m_engine_sound.play_at_pos(0,XFORM().c,sm_Looped);

	setVisible			(TRUE);
	setEnabled			(TRUE);

	return				(TRUE);
}

void CVehicleHelicopter::net_Destroy		()
{
	inherited::net_Destroy();
}

void CVehicleHelicopter::net_Export			(NET_Packet &P)
{
}

void CVehicleHelicopter::net_Import			(NET_Packet &P)
{
}

void CVehicleHelicopter::renderable_Render	()
{
	inherited::renderable_Render();
}

void CVehicleHelicopter::UpdateCL			()
{
	inherited::UpdateCL	();
	m_animator->Update	(Device.fTimeDelta);
	m_engine_sound.set_position(XFORM().c);

#ifndef _DEBUG
	if (m_animator->IsPlaying())
#endif
		XFORM().set	(m_animator->XFORM());
}

void CVehicleHelicopter::shedule_Update(u32 DT)
{
	inherited::shedule_Update	(DT);
	if (!getEnabled())	return;

	Fvector				center;
	float				radius;		
	Center				(center);
	radius				= Radius();
	feel_touch_update	(center,radius);
}

void CVehicleHelicopter::feel_touch_new		(CObject *O)
{
}

void CVehicleHelicopter::feel_touch_delete	(CObject *O)
{
}

BOOL CVehicleHelicopter::feel_touch_contact	(CObject *O)
{
	return				(TRUE);
}
