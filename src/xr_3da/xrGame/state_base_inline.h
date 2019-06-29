////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base_inline.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBaseAbstract CStateBase<_Object>

TEMPLATE_SPECIALIZATION
CStateBaseAbstract::CStateBase			(LPCSTR state_name)
{
	init				(state_name);
}

TEMPLATE_SPECIALIZATION
CStateBaseAbstract::~CStateBase			()
{
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::init			(LPCSTR state_name)
{
	m_object			= 0;
#ifdef LOG_STATE
	m_state_name		= state_name;
//	if (xr_strlen(m_state_name))
//		debug_log		(eStateStateConstructed);
#endif
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::Load			(LPCSTR section)
{
#ifdef LOG_STATE
//	if (xr_strlen(m_state_name))
//		debug_log		(eStateStateLoaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::reinit			(_Object *object)
{
	VERIFY				(object);
	m_object			= object;
	m_inertia_time		= 0;
#ifdef LOG_STATE
	if (xr_strlen(m_state_name))
		debug_log		(eStateStateReinitialized);
#endif
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::reload			(LPCSTR section)
{
#ifdef LOG_STATE
	if (xr_strlen(m_state_name))
		debug_log		(eStateStateReloaded);
#endif
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::initialize		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_state_name))
		debug_log		(eStateStateInitialized);
#endif
	m_start_level_time	= Level().timeServer();
	m_start_game_time	= Level().GetGameTime();
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::execute		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_state_name))
		debug_log		(eStateStateExecuted);
#endif
}

TEMPLATE_SPECIALIZATION
void CStateBaseAbstract::finalize		()
{
#ifdef LOG_STATE
	if (xr_strlen(m_state_name))
		debug_log		(eStateStateFinalized);
#endif
}

TEMPLATE_SPECIALIZATION
bool CStateBaseAbstract::completed		() const
{
	return				(m_start_level_time + m_inertia_time <= Level().timeServer());
}

TEMPLATE_SPECIALIZATION
IC	u32	CStateBaseAbstract::start_level_time			() const
{
	return				(m_start_level_time);
}

TEMPLATE_SPECIALIZATION
IC	ALife::_TIME_ID	CStateBaseAbstract::start_game_time	() const
{
	return				(m_start_game_time);
}

TEMPLATE_SPECIALIZATION
IC	u32	CStateBaseAbstract::inertia_time				() const
{
	return				(m_inertia_time);
}

TEMPLATE_SPECIALIZATION
IC	void CStateBaseAbstract::set_inertia_time			(u32 inertia_time)
{
	m_inertia_time		= inertia_time;
}

#ifdef LOG_STATE
TEMPLATE_SPECIALIZATION
IC	void CStateBaseAbstract::debug_log			(const EStateStates state_state) const
{
	switch (state_state) {
		case eStateStateConstructed : {
			Msg			("[%6d] state %s is constructed",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateLoaded : {
			Msg			("[%6d] state %s is loaded",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateReinitialized : {
			Msg			("[%6d] state %s is reinitialized",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateReloaded : {
			Msg			("[%6d] state %s is reloaded",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateInitialized : {
			Msg			("[%6d] state %s is initialized",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateExecuted : {
			Msg			("[%6d] state %s is executed",Level().timeServer(),m_state_name);
			break;
		}
		case eStateStateFinalized : {
			Msg			("[%6d] state %s is finalized",Level().timeServer(),m_state_name);
			break;
		}
		default : NODEFAULT;
	}
}
#endif

#undef TEMPLATE_SPECIALIZATION
#undef CStateBaseAbstract
