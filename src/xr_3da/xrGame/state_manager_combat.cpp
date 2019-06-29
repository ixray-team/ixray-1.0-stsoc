////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_combat.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager combat
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_combat.h"
#include "ai/stalker/ai_stalker.h"
#include "state_combat_attack_weak.h"
#include "state_combat_cover.h"

//#define TEST

CStateManagerCombat::CStateManagerCombat	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerCombat::~CStateManagerCombat	()
{
}

void CStateManagerCombat::Init			()
{
}

void CStateManagerCombat::Load			(LPCSTR section)
{
#ifndef TEST
	add_state				(xr_new<CStateAttackWeak>("AttackWeak"),	eCombatStateAttackWeak,		0);
#else
	add_state				(xr_new<CStateCover>("Cover"),				eCombatStateCover,			0);
#endif

	inherited::Load			(section);
}

void CStateManagerCombat::reinit		(CAI_Stalker *object)
{
	inherited::reinit		(object);
#ifndef TEST
	set_current_state		(eCombatStateAttackWeak);
	set_dest_state			(eCombatStateAttackWeak);
#else
	set_current_state		(eCombatStateCover);
	set_dest_state			(eCombatStateCover);
#endif
}

void CStateManagerCombat::reload		(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerCombat::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStateManagerCombat::execute		()
{
#ifndef TEST
	set_dest_state			(eCombatStateAttackWeak);
#else
	set_dest_state			(eCombatStateCover);
#endif
	inherited::execute		();
}

void CStateManagerCombat::finalize		()
{
	inherited::finalize		();
}
