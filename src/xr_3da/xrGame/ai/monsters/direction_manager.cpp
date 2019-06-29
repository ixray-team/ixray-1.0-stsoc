#include "stdafx.h"
#include "direction_manager.h"
#include "basemonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "ai_monster_movement.h"
#include "ai_monster_utils.h"
#include "../../level_navigation_graph.h"
#include "../../ai_space.h"
#include "../../ai_object_location.h"
#include "critical_action_info.h"

void CDirectionManager::reinit()
{
	m_delay				= 0;
	m_time_last_faced	= 0;

	m_heading.init		();
	m_heading.current	= m_object->movement().m_body.current.yaw;
	m_heading.target	= m_object->movement().m_body.target.yaw;

	m_pitch.init		();	
	m_pitch.current		= m_object->movement().m_body.current.pitch;
	m_pitch.target		= m_object->movement().m_body.target.pitch;
	
}

void CDirectionManager::face_target(const Fvector &position, u32 delay)
{
	if (m_object->CriticalActionInfo->is_turn_locked()) return;
	
	if (m_time_last_faced + delay > Device.dwTimeGlobal) return;
	
	m_delay = delay;

	float	yaw, pitch;
	Fvector dir;

	dir.sub		(position, m_object->Position());
	dir.getHP	(yaw,pitch);
	yaw			*= -1;
	yaw			= angle_normalize(yaw);
	
	m_heading.target	= yaw;

	m_time_last_faced	= Device.dwTimeGlobal;
}
void CDirectionManager::face_target(const CObject *obj,	u32 delay) 
{
	face_target	(obj->Position(), delay);
}

void CDirectionManager::use_path_direction(bool reversed)
{
	if (m_object->CriticalActionInfo->is_turn_locked()) return;

	float yaw,pitch;
	m_object->movement().detail().direction().getHP	(yaw,pitch);

	if (fsimilar(yaw,0.f,EPS_S)) return;

	m_heading.target = angle_normalize((reversed) ? (-yaw + PI) : (-yaw));
}

void CDirectionManager::set_heading_speed(float value, bool force) 
{
	if (!force && m_object->CriticalActionInfo->is_turn_locked()) return;

	m_heading.speed.target = value;
}

void CDirectionManager::set_heading(float value, bool force) 
{
	if (!force && m_object->CriticalActionInfo->is_turn_locked()) return;

	m_heading.target = value;
}

bool CDirectionManager::is_face_target(const Fvector &position, float eps_angle)
{
	float target_h	= Fvector().sub(position, m_object->Position()).getH();
	float my_h		= m_object->Direction().getH();

	if (angle_difference(target_h,my_h) > eps_angle) return false;
	
	return true;
}

bool CDirectionManager::is_face_target(const CObject *obj, float eps_angle) 
{
	return is_face_target(obj->Position(), eps_angle);
}

bool CDirectionManager::is_from_right(const Fvector &position)
{
	float			yaw, pitch;
	Fvector().sub	(position, m_object->Position()).getHP(yaw,pitch);
	yaw				*= -1;

	return (from_right(yaw,m_heading.current));
}
bool CDirectionManager::is_from_right(float yaw)
{
	return (from_right(yaw,m_heading.current));
}

bool CDirectionManager::is_turning()
{
	return (!fsimilar(m_heading.current,m_heading.target));
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDirectionManager::update_frame()
{
	pitch_correction			();	

	// difference
	float diff = angle_difference(m_pitch.current, m_pitch.target) * 4.0f;
	clamp(diff, PI_DIV_6, 5 * PI_DIV_6);

	m_pitch.speed.target = m_pitch.speed.current = diff;
	
	// поправка угловой скорости в соответствии с текущей и таргетовой линейной скоростями
	// heading speed correction
	if (!fis_zero(m_object->movement().linear_velocity_current()) && !fis_zero(m_object->movement().linear_velocity_target()))
		m_heading.speed.target	= m_heading.speed.target * m_object->movement().linear_velocity_current() / (m_object->movement().linear_velocity_target() + EPS_L);

	// update heading
	velocity_lerp				(m_heading.speed.current, m_heading.speed.target, m_heading.acceleration, Device.fTimeDelta);
	
	m_heading.current			= angle_normalize(m_heading.current);
	m_heading.target			= angle_normalize(m_heading.target);
	angle_lerp					(m_heading.current, m_heading.target, m_heading.speed.current, Device.fTimeDelta);
	
	// update pitch
	velocity_lerp				(m_pitch.speed.current, m_pitch.speed.target, m_pitch.acceleration, Device.fTimeDelta);

	m_pitch.current				= angle_normalize_signed	(m_pitch.current);
	m_pitch.target				= angle_normalize_signed	(m_pitch.target);
	
	angle_lerp					(m_pitch.current, m_pitch.target, m_pitch.speed.current, Device.fTimeDelta);

	// set
	m_object->movement().m_body.speed			= m_heading.speed.current;
	m_object->movement().m_body.current.yaw		= m_heading.current;
	m_object->movement().m_body.target.yaw		= m_heading.current;
	m_object->movement().m_body.current.pitch	= m_pitch.current;
	m_object->movement().m_body.target.pitch	= m_pitch.current;

	// set object position
	Fvector P					= m_object->Position();
	m_object->XFORM().setHPB	(-m_object->movement().m_body.current.yaw,-m_object->movement().m_body.current.pitch,0);
	m_object->Position()		= P;
}

void CDirectionManager::pitch_correction()
{
	CLevelGraph::SContour	contour;
	ai().level_graph().contour(contour, m_object->ai_location().level_vertex_id());

	Fplane				P;
	P.build				(contour.v1,contour.v2,contour.v3);

	Fvector				position_on_plane;
	P.project			(position_on_plane,m_object->Position());

	// находим проекцию точки, лежащей на векторе текущего направления
	Fvector				dir_point, proj_point;
	dir_point.mad		(position_on_plane, m_object->Direction(), 1.f);
	P.project			(proj_point,dir_point);

	// получаем искомый вектор направления
	Fvector				target_dir;
	target_dir.sub		(proj_point,position_on_plane);

	float				yaw,pitch;
	target_dir.getHP	(yaw,pitch);

	m_pitch.target		= -pitch;
}

