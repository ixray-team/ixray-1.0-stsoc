#include "stdafx.h"
#include "ai_monster_movement.h"
#include "ai_monster_utils.h"
#include "biting/ai_biting.h"
#include "ai_monster_motion_stats.h"
#include "../phmovementcontrol.h"
#include "ai_monster_debug.h"
#include "../cover_evaluators.h"

//////////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////////

CMonsterMovement::CMonsterMovement()
{
	m_selector_approach		= xr_new<CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_cover_approach		= xr_new<CCoverEvaluatorCloseToEnemy>(this);
	
	m_object				= 0;

	m_dwFrameReinit			= u32(-1);
	m_dwFrameLoad			= u32(-1);
}

CMonsterMovement::~CMonsterMovement()
{
	xr_delete(m_selector_approach);
	xr_delete(MotionStats);
	xr_delete(m_cover_approach);
}

void CMonsterMovement::Load(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;
	
	inherited::Load(section);

	m_selector_approach->Load(section,"selector_approach");
}

void CMonsterMovement::reinit()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	inherited::reinit();

	b_try_min_time					= false;
	time_last_approach				= 0;
	b_use_dest_orient				= false;

	m_velocity_linear.set			(0.f,0.f);
	m_velocity_angular				= 0.f;

	initialize_movement				();
}

void CMonsterMovement::InitExternal(CAI_Biting *pM)
{
	m_object	= pM;
	VERIFY		(m_object);

	MotionStats	= xr_new<CMotionStats> (m_object);
}

//////////////////////////////////////////////////////////////////////////
// Services

void CMonsterMovement::InitSelector(CAbstractVertexEvaluator &S, Fvector target_pos)
{
	S.m_dwCurTime		= m_object->m_dwCurrentTime;
	S.m_tMe				= m_object;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();
	S.m_dwStartNode		= level_vertex_id();
	S.m_tStartPosition	= Position();

	S.m_tEnemyPosition	= target_pos;
	S.m_tEnemy			= 0;
}

//////////////////////////////////////////////////////////////////////////
// Move To Target
void CMonsterMovement::MoveToTarget(const CEntity *entity) 
{
	SetPathParams(entity->level_vertex_id(), get_valid_position(entity, entity->Position()));
}

void CMonsterMovement::MoveToTarget(const Fvector &pos, u32 node_id) 
{
	if (accessible(node_id)) {
		SetPathParams(node_id,pos);
	} else {
		Fvector res;
		u32 node = accessible_nearest(pos, res);
		SetPathParams(node,res);
	}
	
}


void CMonsterMovement::MoveToTarget(const Fvector &position)
{

	bool new_params = false;

	// перестраивать если дошёл до конца пути
	if (IsPathEnd(2,1.5f)) new_params = true;

	// перестраивать если вышел временной квант
	if (m_selector_approach->m_dwCurTime  + 2000 < m_object->m_dwCurrentTime) new_params = true;

	if (!new_params) return;

	// если нода в прямой видимости - не использовать селектор
	CRestrictedObject::add_border		(Position(), position);
	u32 node_id = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),position);
	CRestrictedObject::remove_border	();
	if (ai().level_graph().valid_vertex_id(node_id) && accessible(node_id)) {
		SetPathParams (node_id, position);

		// хранить в данном селекторе время последнего перестраивания пути
		m_selector_approach->m_dwCurTime = m_object->m_dwCurrentTime;
	} else {
		bool use_selector = true;

		if (ai().level_graph().valid_vertex_position(position) && accessible(position)) {
			u32 vertex_id = ai().level_graph().vertex_id(position);
			if (ai().level_graph().valid_vertex_id(vertex_id) && accessible(vertex_id)) {
				SetPathParams (vertex_id, position);

				// хранить в данном селекторе время последнего перестраивания пути
				m_selector_approach->m_dwCurTime = m_object->m_dwCurrentTime;

				use_selector = false;
			}
		}

		if (use_selector) {
			CLevelLocationSelector::set_evaluator(m_selector_approach);
			InitSelector(*m_selector_approach, position);

			CLevelLocationSelector::set_query_interval(0);	
			SetSelectorPathParams ();
		}
	}
}

void CMonsterMovement::MoveAwayFromTarget(const Fvector &position, float dist)
{
	Fvector target_pos;
	Fvector dir;
		
	dir.sub(Position(), position);
#pragma todo("Dima to Jim : verify why direction becomes incorrect, pssibly XFORM() and/or position is not valid")
	if (fis_zero(dir.square_magnitude(),EPS_L)) dir.set(0.f,0.f,1.f);
	dir.normalize();
	target_pos.mad(Position(),dir,dist);

	MoveToTarget(target_pos);
}


bool CMonsterMovement::IsMoveAlongPathFinished()
{
	return CDetailPathManager::completed(Position());
}

bool CMonsterMovement::IsMovingOnPath()
{
	return (!IsMoveAlongPathFinished() && CMovementManager::enabled() && b_enable_movement);
}

//-------------------------------------------------------------------------------------------

bool CMonsterMovement::IsPathEnd(u32 n_points)
{
	if (CDetailPathManager::path().empty() || !IsMovingOnPath() || (curr_travel_point_index() + n_points >= CDetailPathManager::path().size())) return true;
	return false;
}

bool CMonsterMovement::IsPathEnd(float dist_to_end)
{
	if (CDetailPathManager::path().size() < 2) return true;

	float cur_dist_to_end = 0.f;
	for (u32 i=CDetailPathManager::curr_travel_point_index(); i<CDetailPathManager::path().size()-1; i++) {
		cur_dist_to_end += CDetailPathManager::path()[i].position.distance_to(CDetailPathManager::path()[i+1].position);
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
	//if (m_object->MotionMan.BadMotionFixed()) return true;

	return false;
}


void CMonsterMovement::SetPathParams(u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	set_dest_position(dest_pos);
	set_path_type (MovementManager::ePathTypeLevelPath);
}

void CMonsterMovement::SetSelectorPathParams()
{
	set_path_type (MovementManager::ePathTypeLevelPath);
	
	// использовать при установке селектора: true - использовать путь найденный селектором, false - селектор находит тольтко ноду, путь строит BuildLevelPath
	use_selector_path(true);

}


void CMonsterMovement::WalkNextGraphPoint()
{
	set_path_type		(MovementManager::ePathTypeGamePath);
	set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);
}

void CMonsterMovement::update_velocity()
{
	// Обновить линейную скорость движения
	float t_accel = ((m_velocity_linear.target < m_velocity_linear.current) ? 
										m_object->MotionMan.accel_get(eAV_Braking) :
										m_object->MotionMan.accel_get(eAV_Accel));
		
	velocity_lerp		(m_velocity_linear.current, m_velocity_linear.target, t_accel, Device.fTimeDelta);
	
	// установить линейную скорость движения
	set_desirable_speed	(m_object->m_fCurSpeed = m_velocity_linear.current);

	// установить угловую скорость движения
	if (!fis_zero(m_velocity_linear.current) && !fis_zero(m_velocity_linear.target))
		m_body.speed	= m_velocity_angular * m_velocity_linear.current / (m_velocity_linear.target + EPS_L);
	else 
		m_body.speed	= m_velocity_angular;
}

void CMonsterMovement::set_dest_direction(const Fvector &dir)
{
	CDetailPathManager::set_dest_direction(dir);
}

void CMonsterMovement::stop_now()
{
	CMovementManager::enable_movement	(false);
	m_object->m_velocity_linear.target	= 0.f;
	m_object->m_velocity_linear.current = 0.f;
}
