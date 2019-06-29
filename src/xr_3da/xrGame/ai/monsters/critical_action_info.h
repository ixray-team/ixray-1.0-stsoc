#pragma once

#define CAF_LockAnimation				(1 << 0)
#define CAF_LockPath					(1 << 1)
#define CAF_LockTurn					(1 << 2)
#define CAF_LockMotion					(1 << 3)
#define CAF_LockFSM						(1 << 4)

class CCriticalActionInfo {
	Flags32					m_type;
public:
	IC				CCriticalActionInfo			() {m_type.zero();}
	IC	void		reinit						() {m_type.zero();}

	IC	bool		is_active					() {return (m_type.is_any(u32(-1)) == TRUE);}
	
	IC	bool		is_animation_locked			() {return (m_type.is(CAF_LockAnimation)== TRUE);}
	IC	bool		is_path_locked				() {return (m_type.is(CAF_LockPath)		== TRUE);}
	IC	bool		is_turn_locked				() {return (m_type.is(CAF_LockTurn)		== TRUE);}
	IC	bool		is_motion_locked			() {return (m_type.is(CAF_LockMotion)	== TRUE);}
	IC	bool		is_fsm_locked				() {return (m_type.is(CAF_LockFSM)		== TRUE);}

	IC	void		set							(u32 flags_add)		{m_type.set	(flags_add,		TRUE);}
	IC	void		clear						(u32 flags_remove)	{m_type.set	(flags_remove,	FALSE);}

	IC	void		change						(u32 flags_add, u32 flags_remove) {
		m_type.set	(flags_add,		TRUE);
		m_type.set	(flags_remove,	FALSE);
	}
};