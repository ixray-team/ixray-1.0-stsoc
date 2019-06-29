////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_management_config.h"

template <
	typename _Object
>
class CStateBase {
protected:
	_Object				*m_object;
	u32					m_start_level_time;
	ALife::_TIME_ID		m_start_game_time;
	u32					m_inertia_time;

	enum EStateStates {
		eStateStateConstructed = u32(0),
		eStateStateLoaded,
		eStateStateReinitialized,
		eStateStateReloaded,
		eStateStateInitialized,
		eStateStateExecuted,
		eStateStateFinalized,
		eStateStateDummy = u32(-1),
	};
#ifdef LOG_STATE
	bool				m_log;
	LPCSTR				m_state_name;
#endif
public:
							CStateBase			(LPCSTR state_name);
	virtual					~CStateBase			();
			void			init				(LPCSTR state_name);
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				(_Object *object);
	virtual	void			reload				(LPCSTR section);
	virtual	void			initialize			();
	virtual	void			execute				();
	virtual	void			finalize			();
	virtual	bool			completed			() const;
#ifdef LOG_STATE
	virtual void			debug_log			(const EStateStates state_state) const;
#endif
	IC		u32				start_level_time	() const;
	IC		ALife::_TIME_ID	start_game_time		() const;
	IC		u32				inertia_time		() const;
	IC		void			set_inertia_time	(u32 inertia_time);
};

#include "state_base_inline.h"