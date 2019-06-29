////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_death.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager death
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_death.h"
#include "ai/stalker/ai_stalker.h"
#include "state_death_recently_dead.h"
#include "state_death_already_dead.h"

CStateManagerDeath::CStateManagerDeath	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerDeath::~CStateManagerDeath	()
{
}

void CStateManagerDeath::Init			()
{
}

void CStateManagerDeath::Load			(LPCSTR section)
{
	add_state				(xr_new<CStateDeathRecentlyDead>("RecentlyDead"),	eDeathStateRecentlyDead,	0);
	add_state				(xr_new<CStateDeathAlreadyDead>("AlreadyDead"),		eDeathStateAlreadyDead,		0);
	add_transition			(eDeathStateRecentlyDead,eDeathStateAlreadyDead,1);
	
	inherited::Load			(section);
}

void CStateManagerDeath::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object);
	set_current_state		(eDeathStateRecentlyDead);
	set_dest_state			(eDeathStateRecentlyDead);
}

void CStateManagerDeath::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerDeath::initialize		()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskAnySound));
}

void CStateManagerDeath::execute		()
{
	set_dest_state			(eDeathStateAlreadyDead);
	inherited::execute		();
}

void CStateManagerDeath::finalize		()
{
	inherited::finalize		();
}