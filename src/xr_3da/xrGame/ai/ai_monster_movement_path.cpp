#include "stdafx.h"
#include "ai_monster_movement.h"
#include "../cover_point.h"
#include "../cover_manager.h"
#include "../cover_evaluators.h"
#include "biting/ai_biting.h"


#define MAX_COVER_DISTANCE		50.f
#define MAX_SELECTOR_DISTANCE	10.f
#define MAX_PATH_DISTANCE		100.f


//////////////////////////////////////////////////////////////////////////
// лимитировать по расстоянию
// если на выходе функции m_intermediate.node != u32(-1) - нода найдена
void CMonsterMovement::get_intermediate() 
{
	m_intermediate.node		= m_target.node;
	
	Fvector	dir;
	if (m_target_type == eMoveToTarget) {
		dir.sub						(m_target.position, Position());
		dir.normalize				();
		m_intermediate.position		= m_target.position;
	} else if (m_target_type == eRetreatFromTarget){
		VERIFY(m_intermediate.node == u32(-1));
		
		dir.sub						(Position(), m_target.position);
		dir.normalize				();
		m_intermediate.position.mad	(Position(), dir, MAX_PATH_DISTANCE - 1.f);
	}
	
	float dist = Position().distance_to(m_intermediate.position);		

	// лимитировать по расстоянию
	if (dist > MAX_PATH_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_PATH_DISTANCE);
		m_intermediate.node			= u32(-1);
		Msg("FIND :: Max distance pos cut");
	} else {
		// если задана нода, то выходим
		if (m_intermediate.node != u32(-1) && accessible(m_intermediate.node)) {
			Msg("FIND :: Direct node");
			return;
		}
	}

	// нода в прямой видимости ?
	Msg("FIND :: Directly?");
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
		m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
		Fvector pos = m_intermediate.position;
		fix_position(pos, m_intermediate.node, m_intermediate.position);
		return;
	}

	// используем прикрытия?
	if (m_cover_info.use_covers) {
		if (dist > MAX_COVER_DISTANCE) {
			m_intermediate.position.mad	(Position(), dir, MAX_COVER_DISTANCE);
			m_intermediate.node			= u32(-1);
			Msg("FIND :: Max cover distance pos cut");
		}

		// нода в прямой видимости ?
		Msg("FIND :: Covers Directly?");
		if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
			m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
			Fvector pos = m_intermediate.position;
			fix_position(pos, m_intermediate.node, m_intermediate.position);
			return;
		}
	}

	// лимитировать по расстоянию
	if (dist > MAX_SELECTOR_DISTANCE) {
		m_intermediate.position.mad	(Position(), dir, MAX_SELECTOR_DISTANCE);
		m_intermediate.node			= u32(-1);
		Msg("FIND :: Max selector distance pos cut");
	}

	// нода в прямой видимости ?
	Msg("FIND :: Selectors Directly?");
	if (position_in_direction(m_intermediate.position, m_intermediate.node)) {
		m_intermediate.position.y = ai().level_graph().vertex_plane_y(m_intermediate.node);
		Fvector pos = m_intermediate.position;
		fix_position(pos, m_intermediate.node, m_intermediate.position);
		return;
	}
}

bool CMonsterMovement::position_in_direction(const Fvector &target, u32 &node) 
{
	// нода в прямой видимости?
	CRestrictedObject::add_border(Position(), target);
	node = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),target);
	CRestrictedObject::remove_border();
	if (ai().level_graph().valid_vertex_id(node) && accessible(node)) {
		Msg("FIND :: Position in direction success");
		return true;
	}

	// искать ноду по прямому запросу
	if (ai().level_graph().valid_vertex_position(target) && accessible(target)) {
		node = ai().level_graph().vertex_id(target);
		if (ai().level_graph().valid_vertex_id(node) && accessible(node)) {
			Msg("FIND :: Direct access success");
			return true;
		}
	}

	node = u32(-1);
	return false;
}

void CMonsterMovement::set_parameters()
{
	// известна нода - устанавливаем параметры
	if (m_intermediate.node != u32(-1)) {
		set_dest_position		(m_intermediate.position);
		set_level_dest_vertex	(m_intermediate.node);
		set_path_type			(MovementManager::ePathTypeLevelPath);
		Msg("SET :: Direct node");
		return;
	}

	// находим с помощью каверов
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_intermediate.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		CCoverPoint	*point = ai().cover_manager().best_cover(Position(),m_cover_info.radius,*m_cover_approach);
		if (point) {
			m_intermediate.node		= point->m_level_vertex_id;
			m_intermediate.position	= point->m_position;	
			set_dest_position		(m_intermediate.position);
			set_level_dest_vertex	(m_intermediate.node);
			set_path_type			(MovementManager::ePathTypeLevelPath);
			Msg("SET :: Cover used");
			return;
		}
	}

	// находим с помощью селектора
	CLevelLocationSelector::set_evaluator		(m_selector_approach);
	CLevelLocationSelector::set_query_interval	(0);
	InitSelector								(*m_selector_approach, m_intermediate.position);
	use_selector_path							(true);		// использовать при установке селектора: true - использовать путь найденный селектором, false - селектор находит тольтко ноду, путь строит BuildLevelPath
	set_path_type								(MovementManager::ePathTypeLevelPath);
	
	Msg("SET :: Selector used");
}


void CMonsterMovement::initialize_movement() 
{
	m_target.node				= u32(-1);
	m_intermediate.node			= u32(-1);

	m_time						= 0;
	m_last_time_path_update		= 0;
	m_distance_to_path_end		= 1.f;
	m_path_end					= false;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_actual					= false;
	m_force_rebuild				= false;
}

//////////////////////////////////////////////////////////////////////////
// Set Target Point Routines
//////////////////////////////////////////////////////////////////////////
#define RANDOM_POINT_DISTANCE	20.f

void CMonsterMovement::set_target_point(const Fvector &position, u32 node)
{
	bool bad_state	= false;
	if (failed())	bad_state = true;
	if (path_end()) bad_state = true;

	// плохое состояние?
	if (bad_state) {
		Msg("Set Target :: Bad State");
		m_force_rebuild = true;

		// если путь завершен или failed - выбрать другую случайную точку
		Fvector	pos_random			= position;	
		Fvector dir; dir.random_dir	();

		pos_random.mad( Position(), dir, RANDOM_POINT_DISTANCE);
		
		// установить m_target.position
		if (!accessible(pos_random)) {
			m_target.node		= accessible_nearest(pos_random, m_target.position);	
		} else {
			m_target.node		= u32(-1);
			m_target.position	= pos_random;
		}
	} else {
		m_target.position	= position;
		m_target.node		= node;
		m_force_rebuild		= false;
	}
	
	m_target_type		= eMoveToTarget;
}

void CMonsterMovement::set_retreat_from_point(const Fvector &position)
{
	bool bad_state = false;
	if (failed())	bad_state = true;
	if (path_end()) bad_state = true;

	// плохое состояние?
	if (bad_state) {
		Msg("Set Retreat :: Bad State");
		m_force_rebuild = true;

		// если путь завершен или failed - выбрать другую случайную точку
		Fvector dir; dir.random_dir	();
		m_target.position.mad( Position(), dir, RANDOM_POINT_DISTANCE);
	} else {
		m_target.position	= position;
		m_force_rebuild		= false;
	}
	
	m_target.node		= u32(-1);
	m_target_type		= eRetreatFromTarget;
}

//////////////////////////////////////////////////////////////////////////
// лимитировать по расстоянию
// если на выходе функции m_intermediate.node != u32(-1) - нода найдена
bool CMonsterMovement::check_build_conditions()
{
	if (m_force_rebuild) return true;

	// проверить на завершение пути
	if (!IsPathEnd(m_distance_to_path_end)) {
		m_path_end	= false;
		m_failed	= false;

		// если время движения по пути не вышло
		if (m_last_time_path_update + m_time > m_object->m_current_update) return false;
		return		true;
	}
	
	if (m_intermediate.position.similar(m_target.position)) {
		m_path_end	= true;
		Msg("PATH END!!!");
		return		false;
	}

	if (CDetailPathManager::actual() || CLevelPathManager::failed() || CDetailPathManager::failed()) {
		m_failed	= true;
		Msg("FAILED!!!");
		return		false;
	}

	// если путь ещё не построен - выход
	if (!CDetailPathManager::actual() && (time_path_built() < m_last_time_path_update)) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////////////////
void CMonsterMovement::fix_position(const Fvector &pos, u32 node, Fvector &res_pos)
{
	VERIFY(accessible(node));

	if (!accessible(pos)) {
		u32	level_vertex_id = accessible_nearest(pos,res_pos);
		VERIFY	(level_vertex_id == node);
	}
}
