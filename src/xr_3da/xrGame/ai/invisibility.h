#pragma once
#include "energy_holder.h"

class CInvisibility : public CEnergyHolder {

	typedef CEnergyHolder inherited;

	u32				m_time_start_blink;
	u32				m_time_last_blink;
	bool			m_blink;
	bool			m_cur_visibility;
	

	// external parameters
	u32				timeBlink;
	u32				timeBlinkInterval;

	bool			m_manual;

public:

	virtual	void	reload					(LPCSTR section);
	virtual	void	reinit					();

	virtual void	frame_update			();

	virtual	void	on_activate				();
	virtual	void	on_deactivate			();
			
	virtual	void	on_change_visibility	(bool b_visibility){}

			void	set_manual_switch		(bool b_man = true);
			void	manual_activate			();
			void	manual_deactivate		();
			bool	is_manual_control		() {return m_manual;}

private:
			void	start_blink				();
			void	stop_blink				();
			void	update_blink			();
};



