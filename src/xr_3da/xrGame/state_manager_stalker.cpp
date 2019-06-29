////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_stalker.h"
#include "state_manager_death.h"
#include "state_manager_no_alife.h"
#include "state_manager_alife.h"
#include "state_manager_combat.h"
#include "ai/stalker/ai_stalker.h"

CStateManagerStalker::CStateManagerStalker	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerStalker::~CStateManagerStalker	()
{
}

void CStateManagerStalker::Init				()
{
}

void CStateManagerStalker::Load				(LPCSTR section)
{
	add_state				(xr_new<CStateManagerDeath>("DeathManager"),	eStalkerStateDeath,		0);
	add_state				(xr_new<CStateManagerNoALife>("NoALife"),		eStalkerStateNoALife,	3);
	add_state				(xr_new<CStateManagerCombat>("Combat"),			eStalkerStateCombat,	1);
//	add						(xr_new<CStateManagerALife>(),					eStalkerStateALife,		2);
	add_transition			(eStalkerStateNoALife,eStalkerStateDeath,1);
	add_transition			(eStalkerStateNoALife,eStalkerStateCombat,1,1);
	
	inherited::Load			(section);
}

void CStateManagerStalker::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object);
	set_current_state		(eStalkerStateNoALife);
	set_dest_state			(eStalkerStateNoALife);
}

void CStateManagerStalker::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerStalker::initialize		()
{
	inherited::initialize	();
}

void CStateManagerStalker::execute			()
{
	if (m_object->g_Alive())
		if (!m_object->enemy())
			set_dest_state	(eStalkerStateNoALife);
		else
			set_dest_state	(eStalkerStateCombat);
	else
		set_dest_state		(eStalkerStateDeath);
	
	inherited::execute		();
}

void CStateManagerStalker::finalize			()
{
	inherited::finalize		();
}
