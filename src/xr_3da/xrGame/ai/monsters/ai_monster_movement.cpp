#include "stdafx.h"
#include "ai_monster_movement.h"
#include "ai_monster_utils.h"
#include "BaseMonster/base_monster.h"
#include "ai_monster_motion_stats.h"
#include "../../phmovementcontrol.h"
#include "../../cover_evaluators.h"
#include "../../game_location_selector.h"
#include "../../level_location_selector.h"
#include "../../detail_path_manager.h"
#include "../../ai_object_location.h"
#include "../../ai_space.h"
#include "ai_monster_movement_space.h"
#include "critical_action_info.h"

using namespace MonsterMovement;

//////////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////////

void CMonsterMovement::detour_graph_points()
{
	set_path_type		(MovementManager::ePathTypeGamePath);
	game_selector().set_selection_type	(eSelectionTypeRandomBranching);
}

CMonsterMovement::CMonsterMovement(CBaseMonster *base_monster) :
	inherited				(base_monster)
{
	VERIFY					(base_monster);
	m_object				= base_monster;
	MotionStats				= xr_new<CMotionStats>(m_object);

	m_selector_approach		= xr_new<CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_cover_approach		= xr_new<CCoverEvaluatorCloseToEnemy>(&restrictions());
}

CMonsterMovement::~CMonsterMovement()
{
	xr_delete(m_selector_approach);
	xr_delete(MotionStats);
	xr_delete(m_cover_approach);
}

void CMonsterMovement::Load(LPCSTR section)
{
	inherited::Load(section);

	m_selector_approach->Load(section,"selector_approach");
	
	load_velocity	(section, "Velocity_Stand",			eVelocityParameterStand);
	load_velocity	(section, "Velocity_WalkFwdNormal",	eVelocityParameterWalkNormal);
	load_velocity	(section, "Velocity_RunFwdNormal",	eVelocityParameterRunNormal);
	load_velocity	(section, "Velocity_WalkFwdDamaged",eVelocityParameterWalkDamaged);
	load_velocity	(section, "Velocity_RunFwdDamaged",	eVelocityParameterRunDamaged);
	load_velocity	(section, "Velocity_Steal",			eVelocityParameterSteal);
	load_velocity	(section, "Velocity_Drag",			eVelocityParameterDrag);
	load_velocity	(section, "Velocity_RunAttack",		eVelocityParameterRunAttack);

	// add idle velocity
	SVelocityParam velocity_param;
	m_velocities.insert(mk_pair(eVelocityParameterIdle, velocity_param));
	detail().add_velocity(eVelocityParameterIdle, CDetailPathManager::STravelParams(velocity_param.velocity.linear,velocity_param.velocity.angular_path,velocity_param.velocity.angular_real));	
}

void CMonsterMovement::reinit()
{
	inherited::reinit();

	b_try_min_time					= false;
	time_last_approach				= 0;
	b_use_dest_orient				= false;

	m_velocity_reset				= false;

	m_velocity_linear.set			(0.f,0.f);

	m_dest_dir.set					(0.f, 0.f, 0.f);
	
	initialize_movement				();
}

void CMonsterMovement::load_velocity(LPCSTR section, LPCSTR line, u32 velocity_id)
{
	SVelocityParam velocity_param;
	if(pSettings->line_exist(section,line)) velocity_param.Load(section,line);
	m_velocities.insert(mk_pair(velocity_id, velocity_param));

	detail().add_velocity(velocity_id, CDetailPathManager::STravelParams(velocity_param.velocity.linear,velocity_param.velocity.angular_path,velocity_param.velocity.angular_real));	
}

SVelocityParam &CMonsterMovement::get_velocity(u32 velocity_id)
{
	VELOCITY_MAP_IT it = m_velocities.find(velocity_id);
	VERIFY(it != m_velocities.end());

	return it->second;
}


//////////////////////////////////////////////////////////////////////////
// Services

void CMonsterMovement::InitSelector(CAbstractVertexEvaluator &S, Fvector target_pos)
{
	S.m_dwCurTime		= object().m_dwCurrentTime;
	S.m_tMe				= m_object;
	S.m_tpMyNode		= object().ai_location().level_vertex();
	S.m_tMyPosition		= object().Position();
	S.m_dwStartNode		= object().ai_location().level_vertex_id();
	S.m_tStartPosition	= object().Position();
	S.m_tEnemyPosition	= target_pos;
	S.m_tEnemy			= 0;
}

bool CMonsterMovement::IsMoveAlongPathFinished()
{
	return (detail().completed(object().Position()));
}

bool CMonsterMovement::IsMovingOnPath()
{
	return (!IsMoveAlongPathFinished() && enabled() && b_enable_movement);
}

//-------------------------------------------------------------------------------------------

bool CMonsterMovement::IsPathEnd(u32 n_points)
{
	if (detail().path().empty() || !IsMovingOnPath() || (detail().curr_travel_point_index() + n_points >= detail().path().size())) return true;
	return false;
}

bool CMonsterMovement::IsPathEnd(float dist_to_end)
{
	if (detail().path().size() < 2) return true;
	if (path_completed())						 return true;

	float cur_dist_to_end = 0.f;
	for (u32 i=detail().curr_travel_point_index(); i<detail().path().size()-1; i++) {
		cur_dist_to_end += detail().path()[i].position.distance_to(detail().path()[i+1].position);
		if (cur_dist_to_end > dist_to_end) break;
	}

	if (!IsMovingOnPath() || (cur_dist_to_end < dist_to_end)) return true;
	return false;
}

bool CMonsterMovement::IsPathEnd(u32 n_points, float dist_to_end)
{
	return (IsPathEnd(n_points) || IsPathEnd(dist_to_end));
}

//-------------------------------------------------------------------------------------------


bool CMonsterMovement::ObjectNotReachable(const CEntity *entity) 
{
	if (!object_position_valid(entity)) return true;
	
	// Commented because of Acceleration Conflict
	//if (object()MotionMan.BadMotionFixed()) return true;

	return false;
}


void CMonsterMovement::SetPathParams(u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	detail().set_dest_position(dest_pos);
	set_path_type (MovementManager::ePathTypeLevelPath);
}

void CMonsterMovement::SetSelectorPathParams()
{
	set_path_type (MovementManager::ePathTypeLevelPath);
	
	// использовать при установке селектора: true - использовать путь найденный селектором, false - селектор находит тольтко ноду, путь строит BuildLevelPath
	use_selector_path(true);

}

void CMonsterMovement::update_velocity()
{
	// Обновить линейную скорость движения
	float t_accel = ((m_velocity_linear.target < m_velocity_linear.current) ? 
										object().MotionMan.accel_get(eAV_Braking) :
										object().MotionMan.accel_get(eAV_Accel));
		
	velocity_lerp		(m_velocity_linear.current, m_velocity_linear.target, t_accel, Device.fTimeDelta);
	
	// установить линейную скорость движения
	set_desirable_speed	(object().m_fCurSpeed = m_velocity_linear.current);
}

void CMonsterMovement::set_dest_direction(const Fvector &dir)
{
	m_dest_dir.set	(dir);
}

void CMonsterMovement::stop_now()
{
	enable_movement						(false);
	disable_path						();
	set_linear_velocity					(0.f, true);
	
	clear_path							();
}

//////////////////////////////////////////////////////////////////////////
// Special Build Path
//////////////////////////////////////////////////////////////////////////
bool CMonsterMovement::build_special(const Fvector &target, u32 node, u32 vel_mask, bool linear)
{
	if (node == u32(-1)) {
		// нода в прямой видимости?
		restrictions().add_border(object().Position(), target);
		node = ai().level_graph().check_position_in_direction(object().ai_location().level_vertex_id(),object().Position(),target);
		restrictions().remove_border();
		
		if (!ai().level_graph().valid_vertex_id(node) || !accessible(node)) return false;
	}
	
	enable_movement						(true);
	
	detail().set_velocity_mask			(vel_mask);	
	detail().set_desirable_mask			(vel_mask);

	detail().set_try_min_time			(false); 
	detail().set_use_dest_orientation	(false);
	
	level_selector().set_evaluator		(0);
	detail().set_path_type				(eDetailPathTypeSmooth);
	set_path_type						(MovementManager::ePathTypeLevelPath);

	detail().set_dest_position			(target);
	set_level_dest_vertex				(node);
	
	set_build_path_at_once				();
	update_path							();	
	
	if (is_path_built())				return true;

	enable_movement						(false);
	return false;
}

//////////////////////////////////////////////////////////////////////////

float CMonsterMovement::get_path_angle()
{
	if (detail().path().size() < 3) return 0.f;

	Fvector		dir;
	float		h1,h2,p;

	dir.sub		(detail().path()[1].position, detail().path()[0].position);
	dir.getHP	(h1,p);
	dir.sub		(detail().path()[detail().path().size() - 1].position, detail().path()[0].position);
	dir.getHP	(h2,p);

	return	(angle_difference(h1,h2));
}

bool CMonsterMovement::is_path_built()
{
	return (!path_completed() && (detail().time_path_built() >= Device.dwTimeGlobal));
}

void CMonsterMovement::set_velocity_from_path() 
{
	if (!IsMovingOnPath()) return;
	
	u32 cur_point_velocity_index	= detail().path()[detail().curr_travel_point_index()].velocity;
	u32 next_point_velocity_index	= u32(-1);

	if (detail().path().size() > detail().curr_travel_point_index() + 1) 
		next_point_velocity_index = detail().path()[detail().curr_travel_point_index() + 1].velocity;

	const CDetailPathManager::STravelParams &current_velocity	= detail().velocity(cur_point_velocity_index);
	if (fis_zero(current_velocity.linear_velocity) && (next_point_velocity_index != u32(-1))) {
		const CDetailPathManager::STravelParams &next_velocity	= detail().velocity(next_point_velocity_index);
		set_linear_velocity					(_abs(next_velocity.linear_velocity));
		m_object->DirMan.set_heading_speed	(next_velocity.real_angular_velocity);
	} else {
		set_linear_velocity					(_abs(current_velocity.linear_velocity));
		m_object->DirMan.set_heading_speed(current_velocity.real_angular_velocity);
	}

	if (fis_zero(m_velocity_linear.target)) stop_linear();
}

// Get Point From Node in Radius
bool CMonsterMovement::GetNodeInRadius(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node)
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

void CMonsterMovement::on_travel_point_change()
{
	if (IsMovingOnPath()) {
		u32 cur_point_velocity_index = detail().path()[detail().curr_travel_point_index()].velocity;		
		if ((cur_point_velocity_index == eVelocityParameterStand) && !fis_zero(m_velocity_linear.current) && !m_velocity_reset) {
			stop_linear			();
			m_velocity_reset	= true;
		}
		if (cur_point_velocity_index != eVelocityParameterStand) m_velocity_reset = false;
	}
}

void CMonsterMovement::set_linear_velocity(float value, bool force)
{
	if (m_object->CriticalActionInfo->is_motion_locked()) return;
	
	m_velocity_linear.target				= value;
	if (force) m_velocity_linear.current	= value;
}
