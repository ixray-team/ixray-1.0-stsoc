////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_cover.cpp
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state cover from enemy
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_combat_cover.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory_item.h"
#include "missile.h"
#include "cover_manager.h"
#include "cover_point.h"

CStateCover::CStateCover			(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateCover::~CStateCover			()
{
}

void CStateCover::Init				()
{
}

void CStateCover::Load				(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateCover::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
	m_nearest.reserve				(100);
}

void CStateCover::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateCover::initialize		()
{
	inherited::initialize			();
	m_object->set_refresh_rate		(1500);
}

void CStateCover::finalize		()
{
	inherited::finalize				();
	m_object->set_refresh_rate		(0);
}

void CStateCover::execute			()
{
	inherited::execute				();
	
	if (!m_object->enemy())
		return;
	
	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!dynamic_cast<CMissile*>(m_object->best_weapon())) {
#ifdef OLD_OBJECT_HANDLER
		m_object->CObjectHandler::set_dest_state(eObjectActionAim1,m_object->best_weapon());
#else
		m_object->CObjectHandlerGOAP::set_goal	(eObjectActionAim1,m_object->best_weapon());
#endif
	}
	
	mem_object.m_object_params.m_position		= m_object->enemy()->Position();
	Fvector										direction;
	direction.sub								(mem_object.m_object_params.m_position,m_object->Position());
	m_object->setup								(SightManager::eSightTypeDirection,&direction);

	ai().cover_manager().covers().nearest		(m_object->Position(),30.f,m_nearest);
	float										best_value = flt_max;
	CCoverPoint									*best_point = 0;
	xr_vector<CCoverPoint*>::const_iterator	I = m_nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = m_nearest.end();
	for ( ; I != E; ++I) {
		Fvector			direction;
		float			y,p;
		direction.sub	(mem_object.m_object_params.m_position,(*I)->position());
		direction.getHP	(y,p);
		float			cover_value = ai().level_graph().cover_in_direction(y,(*I)->level_vertex_id());
		float			enemy_distance = mem_object.m_object_params.m_position.distance_to((*I)->position());
		float			my_distance = m_object->Position().distance_to((*I)->position());
		if ((cover_value < best_value) && (enemy_distance > 5.f) && (my_distance < 30.f)){
			best_value	= cover_value;
			best_point	= *I;
		}
	}
	
	if (!best_point)
		return;

	m_object->set_level_dest_vertex	(best_point->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&best_point->position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
}
