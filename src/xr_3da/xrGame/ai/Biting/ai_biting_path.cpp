////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../graph_engine.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"
#include "../corpse_cover.h"
#include "../../cover_manager.h"
#include "../../cover_point.h"

// Развернуть объект в направление движения по пути
void CAI_Biting::SetDirectionLook(bool bReversed)
{
	float yaw,pitch;
	
	// get prev
	CDetailPathManager::direction().getHP(yaw,pitch);

	if (fsimilar(yaw,0.f,EPS_S)) return;

	m_body.target.yaw = -yaw;

	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
	else m_body.target.yaw = angle_normalize(m_body.target.yaw);
}

// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
void CAI_Biting::LookPosition(Fvector to_point, float angular_speed)
{
	// по-умолчанию просто изменить m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,p;
	dir.getHP(yaw,p);

	// установить текущий угол
	m_body.target.yaw = angle_normalize(-yaw);
}


void CAI_Biting::FaceTarget(const CEntity *entity) 
{
	float yaw, pitch;
	Fvector dir;
	
	dir.sub(entity->Position(), Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;

}

void CAI_Biting::FaceTarget(const Fvector &position) 
{
	float yaw, pitch;
	Fvector dir;

	dir.sub(position, Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;
}

void CAI_Biting::on_travel_point_change()
{
	if (IsMovingOnPath()) {
		u32 cur_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;		
		if ((cur_point_velocity_index == eVelocityParameterStand) && !fis_zero(m_velocity_linear.current) && !b_velocity_reset) {
			stop_linear		();
			b_velocity_reset = true;
		}
		if (cur_point_velocity_index != eVelocityParameterStand) b_velocity_reset = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Covers
//////////////////////////////////////////////////////////////////////////

bool CAI_Biting::GetCorpseCover(Fvector &position, u32 &vertex_id) 
{
	m_corpse_cover_evaluator->setup(10.f,50.f);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),30.f,*m_corpse_cover_evaluator);
	if (!point) return false;
	
	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CAI_Biting::GetCoverFromEnemy(const Fvector &enemy_pos, Fvector &position, u32 &vertex_id) 
{
	m_enemy_cover_evaluator->setup(enemy_pos, 30.f,50.f);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),40.f,*m_enemy_cover_evaluator);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CAI_Biting::GetCoverFromPoint(const Fvector &pos, Fvector &position, u32 &vertex_id, float min_dist, float max_dist, float radius) 
{
	m_enemy_cover_evaluator->setup(pos, min_dist,max_dist);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),radius,*m_enemy_cover_evaluator);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

bool CAI_Biting::GetCoverCloseToPoint(const Fvector &dest_pos, float min_dist, float max_dist, float deviation, float radius ,Fvector &position, u32 &vertex_id) 
{
	m_cover_evaluator_close_point->setup(dest_pos,min_dist, max_dist,deviation);
	CCoverPoint	 *point = ai().cover_manager().best_cover(Position(),radius,*m_cover_evaluator_close_point);
	if (!point) return false;

	position	= point->m_position;
	vertex_id	= point->m_level_vertex_id;
	return true;
}

// Get Point From Node in Radius
bool CAI_Biting::GetNodeInRadius(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node)
{
	Fvector vertex_position = ai().level_graph().vertex_position(src_node);

	for (u32 i=0; i<attempts; i++) {
		
		Fvector dir;
		dir.random_dir	();
		dir.normalize	();

		Fvector new_pos;
		new_pos.mad(vertex_position, dir, Random.randF(min_radius, max_radius));

		dest_node = ai().level_graph().check_position_in_direction(src_node, vertex_position, new_pos);
		if (dest_node != u32(-1) && accessible(dest_node)) return (true);
	}
	return false;
}
