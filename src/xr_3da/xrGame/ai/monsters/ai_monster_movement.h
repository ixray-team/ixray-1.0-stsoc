#pragma once

#include "../../movement_manager.h"
#include "../../movement_manager_space.h"
#include "ai_monster_defs.h"
#include "ai_monster_motion_stats.h"

class CMotionStats;
class CAbstractVertexEvaluator;
class CBaseMonster;
class CCoverEvaluatorCloseToEnemy;

class CMonsterMovement : public CMovementManager {
	typedef CMovementManager inherited;
public:
	CBaseMonster		*m_object;
	bool				b_try_min_time;
	bool				b_enable_movement;
	bool				b_use_dest_orient;
	Fvector				m_dest_dir;


	TTime				time_last_approach;

	bool				b_targeted_path;		// предположительно путь ведёт к целевой точке

	CMotionStats		*MotionStats;

		bool	IsMoveAlongPathFinished	();
		bool	IsMovingOnPath			();
		bool	ObjectNotReachable		(const CEntity *entity);
	IC	CBaseMonster	&object			() const;

	// -------------------------------------------------------------------

		bool	IsPathEnd				(u32 n_points, float dist_to_end);
		bool	IsPathEnd				(float dist_to_end);
			
	// -------------------------------------------------------------------

	IC	void	set_try_min_time		(bool new_val) {b_try_min_time		= new_val;}
	IC	void	set_use_dest_orient		(bool new_val) {b_use_dest_orient	= new_val;}
	IC	void	disable_path			() {b_enable_movement = false;}
	IC	void	enable_path				() {b_enable_movement = true;}

		void	set_dest_direction		(const Fvector &dir);

	IC 	bool	is_path_targeted		() {return b_targeted_path;}
	IC	void	set_path_targeted		(bool val = true) {b_targeted_path = val;}
	// -------------------------------------------------------------------

		void	stop_now				();
	IC	void	stop_linear				() {m_velocity_linear.set		(0.f,0.f);}	// set current && target to zero
	IC	void	stop_linear_accel		() {m_velocity_linear.target	= 0.f;}		// set target only to zero

	
		void	set_linear_velocity		(float value, bool force = false);
		float	linear_velocity_current	() {return m_velocity_linear.current;}
		float	linear_velocity_target	() {return m_velocity_linear.target;}

public:
						CMonsterMovement	(CBaseMonster *base_monster);
	virtual				~CMonsterMovement	();
	virtual void		reinit				();
	virtual void		Load				(LPCSTR section);	

				
			void		Update_Initialize	();
			void		Update_Execute		();
			void		Update_Finalize		();


			void		update_velocity		();

			
	virtual void		on_travel_point_change();		

			
//private:
	
		// проверка на завершение пути
		bool		IsPathEnd				(u32 n_points);

		void		InitSelector			(CAbstractVertexEvaluator &S, Fvector target_pos);
		void		SetPathParams			(u32 dest_vertex_id, const Fvector &dest_pos);
		void		SetSelectorPathParams	();

		bool		GetNodeInRadius			(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node);

//////////////////////////////////////////////////////////////////////////
private:

		struct STarget {
			Fvector		position;
			u32			node;
			void		init		() {
				position.set	(0.f,0.f,0.f);
				node			= u32(-1);
			}

			void		set			(const Fvector &pos, u32 vertex) {
				position.set	(pos);
				node			= vertex;
			}

		} m_target_set, m_target_selected, m_target_found;

		u32			m_time;					// время перестроения пути
		u32			m_last_time_target_set;
		float		m_distance_to_path_end;
		bool		m_failed;
		bool		m_force_rebuild;
		bool		m_wait_path_end;

		bool		m_target_actual;		// устанавливаемый таргет соответствует предыдущему

		struct {
			bool	use_covers;
			float	min_dist;
			float	max_dist;
			float	deviation;
			float	radius;
		} m_cover_info;

		enum {
			eMoveToTarget,
			eRetreatFromTarget,
		} m_target_type;

		CCoverEvaluatorCloseToEnemy			*m_cover_approach;
		CAbstractVertexEvaluator			*m_selector_approach;

public:		
		void		set_target_point		(const Fvector &position, u32 node = u32(-1));
		void		set_retreat_from_point	(const Fvector &position);

	IC	void		set_rebuild_time		(u32 time);
	IC	void		set_cover_params		(float min, float max, float dev, float radius);
	IC	void		set_use_covers			(bool val = true);
	IC	void		set_distance_to_end		(float dist);

		void		initialize_movement		();
		void		detour_graph_points		();
	IC	void		set_generic_parameters	();

private:
		// functional
		void		update_target_point			();
		void		check_failure				();

		bool		target_point_need_update	();
		void		find_target					();
		void		set_found_target			();

		void		select_target				();		// выбрать 
		
		// utils
		bool		valid_destination			(const Fvector &pos, u32 node);
		void		fix_position				(const Fvector &pos, u32 node, Fvector &res_pos);

		// results	
	IC	bool		failed						();

//////////////////////////////////////////////////////////////////////////

public:
	//////////////////////////////////////////////////////////////////////////
	// special path
		bool		build_special			(const Fvector &target, u32 node, u32 vel_mask, bool linear = true);
	//////////////////////////////////////////////////////////////////////////
	// service methods
	
		float		get_path_angle			();
		bool		is_path_built			();
		void		set_velocity_from_path	();


	//////////////////////////////////////////////////////////////////////////

	DEFINE_MAP		(u32, SVelocityParam, VELOCITY_MAP, VELOCITY_MAP_IT);
	VELOCITY_MAP	m_velocities;

		void			load_velocity	(LPCSTR section, LPCSTR line, u32 param);
		SVelocityParam	&get_velocity	(u32 velocity_id);


private:	
	
	bool				m_velocity_reset;
	SVelocity			m_velocity_linear;
};

#include "ai_monster_movement_inline.h"
