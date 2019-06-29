#pragma once

#include "monster_event_manager_defs.h"
#include "../../../SkeletonAnimated.h"

class CAnimTriple;
class CBaseMonster;

class CJumpingAbility {
	CBaseMonster	*m_object;
	CAnimTriple		*m_animation_holder;

	// loadable parameters
	u32				m_delay_after_jump;
	float			m_jump_factor;
	float			m_trace_ground_range;
	float			m_hit_trace_range;
	float			m_build_line_distance;
	float			m_min_distance;
	float			m_max_distance;
	float			m_max_angle;

	u32				m_time_next_allowed;
	u32				m_time_started;

	MotionID		m_def_glide;
	
	// run-time params
	float			m_jump_time;
	float			m_blend_speed;
	u32				m_velocity_mask;
	Fvector			m_target_position;
	CObject			*m_target_object;

	// state flags
	bool			m_active;
	bool			m_object_hitted;
	bool			m_velocity_bounced;
	bool			m_use_prediction;
	
	bool			m_enable_bounce;

public:
					CJumpingAbility		();
	virtual			~CJumpingAbility	();

	// init on construction
	virtual void	init_external		(CBaseMonster *obj);
	
	// load jump parameters
	virtual void	load				(LPCSTR section);
	
	// init on spawn
	virtual void	reinit				(const MotionID &m_def1, const MotionID &m_def2, const MotionID &m_def3);
	
	// process jump
	virtual void	jump				(CObject *obj, u32 vel_mask = u32(-1), bool skip_prepare = false);
	virtual void	jump				(const Fvector &point, u32 vel_mask = u32(-1), bool skip_prepare = false);
	virtual void	update_frame		();

	// check for distance and angle difference
	virtual	bool	can_jump			(CObject *target);

	virtual	bool	active				() {return m_active;}

	// stop/break jump and all of jumping states
	virtual void	stop				();

			void	disable_bounce		() {m_enable_bounce = false;}

private:	
	// service routines		
			// build path after jump 
			void	build_line			();
			// get target position according to object center point
			Fvector get_target			(CObject *obj);
			// finalize jump
			void	pointbreak			();
			// set current blend speed
			void	set_animation_speed	();
			// check for hit object
			void	hit_test			();

	// define global callbacks
	DEFINE_DELEGATE	on_TA_change		(IEventData *p_data);
	DEFINE_DELEGATE	on_velocity_bounce	(IEventData *p_data);

	// check current jump state		
			bool	is_landing			();	
			bool	is_on_the_ground	();

	// position prediction
			Fvector	predict_position	(CObject *obj, const Fvector &pos);

			void	start_jump			(const Fvector &point, bool skip_prepare);
};
