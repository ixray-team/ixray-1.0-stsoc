////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sound action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sound_action.h"
#include "sound_player.h"
#include "ai/stalker/ai_stalker.h"

CSoundAction::~CSoundAction	()
{
	finalize						();
}

void CSoundAction::initialize		()
{
	if (m_use_init_mask)
		object().sound().set_sound_mask	(m_init_mask);
}

void CSoundAction::execute			()
{
	if (!m_use_params)
		return;
	
	object().sound().play					(
		m_type,
		m_max_start_time,
		m_min_start_time,
		m_max_stop_time,
		m_min_stop_time,
		m_id
	);
}

void CSoundAction::finalize			()
{
	if (m_use_finish_mask)
		object().sound().set_sound_mask	(m_finish_mask);
}