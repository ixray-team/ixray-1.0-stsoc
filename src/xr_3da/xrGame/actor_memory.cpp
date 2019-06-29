////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory.cpp
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor_memory.h"
#include "visual_memory_manager.h"
#include "actor.h"
#include "../camerabase.h"
#include "gamepersistent.h"
#include "level.h"

CActorMemory::CActorMemory					(CActor *actor)
{
	m_visual								= xr_new<CVisualMemoryManager>((CCustomMonster*)0,(CAI_Stalker*)0,actor);
	m_actor									= actor;
	m_state									= 0;
}

CActorMemory::~CActorMemory					()
{
	shedule_unregister						();
	xr_delete								(m_visual);
}

BOOL CActorMemory::feel_vision_isRelevant	(CObject* O)
{
	CEntityAlive							*entity_alive = smart_cast<CEntityAlive*>(O);
	if (!entity_alive)
		return								(FALSE);

	return									(TRUE);
}

float CActorMemory::feel_vision_mtl_transp	(CObject* O, u32 element)
{
	return									(visual().feel_vision_mtl_transp(O,element));
}

float CActorMemory::shedule_Scale			()
{
	return									(0.f);
}

void CActorMemory::eye_pp_s01				()
{
	// Standart visibility
	Device.Statistic->AI_Vis_Query.Begin		();
	
	Fvector									c,k,j;
	m_actor->cam_Active()->Get				(c,k,j);

	Fmatrix									mProject,mFull,mView;
	mView.build_camera_dir					(c,k,j);
	m_position								= c;
	mProject.build_projection				(deg2rad(m_actor->cam_Active()->f_fov),m_actor->cam_Active()->f_aspect,0.1f,g_pGamePersistent->Environment.CurrentEnv.far_plane);
	mFull.mul								(mProject,mView);
	feel_vision_query						(mFull,c);
	Device.Statistic->AI_Vis_Query.End		();
}

void CActorMemory::eye_pp_s2				()
{
	// Tracing
	Device.Statistic->AI_Vis_RayTests.Begin	();
	u32										dwTime = Level().timeServer();
	u32										dwDT = dwTime - m_time_stamp;
	m_time_stamp							= dwTime;
	feel_vision_update						(m_actor,m_position,float(dwDT)/1000.f,visual().transparency_threshold());
	Device.Statistic->AI_Vis_RayTests.End	();
}

void CActorMemory::shedule_Update			(u32 dt)
{
	inherited::shedule_Update	(dt);

	if (!object().g_Alive())
		return;
	
//	if (!object().Local())
//		return;

	switch (m_state) {
		case 0 : {
			m_state							= 1;
			eye_pp_s01						();
			break;
		}
		case 1 : {
			m_state							= 0;
			eye_pp_s2						();
			break;
		}
		default								: NODEFAULT;
	}

	visual().update							(float(dt)/1000.f);

#if 0
	CVisualMemoryManager::VISIBLES::const_iterator	I = visual().objects().begin();
	CVisualMemoryManager::VISIBLES::const_iterator	E = visual().objects().end();
	for ( ; I != E; ++I)
		if (visual().visible_now((*I).m_object))
			Msg								("%6d actor sees %s",Device.dwTimeGlobal,*(*I).m_object->cName());
#endif
}

shared_str CActorMemory::shedule_Name		() const
{
	return									("actor_memory");
}

void CActorMemory::Load						(LPCSTR section)
{
	shedule.t_min							= 100;//_max(READ_IF_EXISTS(pSettings,r_u32,section,"memory_update_time",50),1);
	shedule.t_max							= shedule.t_min;
	shedule_register						();

	visual().Load							(section);
}

void CActorMemory::reinit					()
{
	visual().reinit							();
}

void CActorMemory::reload					(LPCSTR section)
{
	visual().reload							(section);
}

void CActorMemory::remove_links				(CObject *object)
{
	visual().remove_links					(object);
}
