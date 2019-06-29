////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sound action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "control_action.h"

class CSoundAction : public CControlAction {
protected:
	bool		m_use_init_mask;
	u32			m_init_mask;
	
	bool		m_use_finish_mask;
	u32			m_finish_mask;
	
	bool		m_use_params;
	u32			m_type;
	u32			m_max_start_time;
	u32			m_min_start_time;
	u32			m_max_stop_time;
	u32			m_min_stop_time;
	u32			m_id;

public:
	IC						CSoundAction	();
	virtual					~CSoundAction	();
	IC						CSoundAction	(u32 internal_type, u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
			void			initialize		();
			void			execute			();
			void			finalize		();
	IC		void			set_init_mask	(u32 init_mask);
	IC		void			set_finish_mask	(u32 finish_mask);
};

#include "sound_action_inline.h"
