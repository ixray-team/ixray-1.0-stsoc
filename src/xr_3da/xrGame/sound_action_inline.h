////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sound action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSoundAction::CSoundAction	()
{
	m_use_init_mask		= false;
	m_use_finish_mask	= false;
	m_use_params		= false;
}

IC	CSoundAction::CSoundAction	(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	m_use_params		= false;
	m_type				= internal_type;
	m_max_start_time	= max_start_time;
	m_min_start_time	= min_start_time;
	m_max_stop_time		= max_stop_time;
	m_min_stop_time		= min_stop_time;
	m_id				= id;
}

IC	void CSoundAction::set_init_mask	(u32 init_mask)
{
	m_use_init_mask		= true;
	m_init_mask			= init_mask;
}

IC	void CSoundAction::set_finish_mask	(u32 finish_mask)
{
	m_use_finish_mask	= true;
	m_finish_mask		= finish_mask;
}

