////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
class CStateManager {
	xr_map<u32,T*>		m_states;
	u32					m_cur_state;
	u32					m_dest_state;
	bool				m_actuality;
public:
						CStateManager		();
	virtual				~CStateManager		();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				();
	virtual	void		reload				(LPCSTR section);

			void		add					(T *state, u32 state_id);
			void		remove				(u32 state_id);
			void		update				(u32 time_delta);

	IC		u32			current_state		() const;
	IC		u32			dest_state			() const;
	IC		void		set_dest_state		(u32 state_id);
};

#include "state_manager_inline.h"