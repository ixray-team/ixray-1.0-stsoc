#pragma once

template<typename _Object>
class CScanningAbility {
	_Object		*object;

	// external members
	float		critical_value;
	float		scan_radius;
	float		velocity_threshold;
	float		decrease_value;
	
	ref_sound	sound_scan;

	// interanal members
	enum {
		eStateDisabled,
		eStateNotActive,
		eStateScanning
	} state;
	
	float		scan_value;

public:
			void	init_external			(_Object *obj) {object = obj;}

			void	load					(LPCSTR section);
	virtual	void	reinit					();
		
			void	schedule_update			();
			void	frame_update			(u32 dt);

			void	enable					();
			void	disable					();

	virtual void	on_scan_success			() {}
	virtual void	on_scanning				() {}

private:
			float	get_velocity			(CObject *obj);
};

#include "scanning_ability_inline.h"