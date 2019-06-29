#pragma once

class CBaseMonster;

class CDirectionManager {
	CBaseMonster	*m_object;

	u32				m_time_last_faced;
	u32				m_delay;

public:	
	struct SAxis {
		float	current;
		float	target;
		float	acceleration;

		struct {
			float current;
			float target;
		} speed;
			
		void	init	() {
			current			= 0;
			target			= 0;
			speed.current	= 0;
			speed.target	= 0;
			acceleration	= flt_max;
		}
	} m_heading, m_pitch;

public:
	IC			void	init_external			(CBaseMonster *obj) {m_object = obj;}
				void	reinit					();

				void	update_frame			();

				void	use_path_direction		(bool reversed = false);

				void	face_target				(const Fvector &position,	u32 delay = 0);
				void	face_target				(const CObject *obj,		u32 delay = 0);
	IC			void	set_delay				(u32 delay) {m_delay = delay;}

				void	set_heading_speed		(float value, bool force = false);
				void	set_heading				(float value, bool force = false);

	IC	const	SAxis	&heading				() {return m_heading;}		

				bool	is_face_target			(const Fvector &position,	float eps_angle);
				bool	is_face_target			(const CObject *obj,		float eps_angle);
				
				bool	is_from_right			(const Fvector &position);
				bool	is_from_right			(float yaw);

				bool	is_turning				();
private:				
				void	pitch_correction		();
};
