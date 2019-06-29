////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state.cpp
//	Created 	: 16.01.2004
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"
#include "missile.h"

CObjectStateBase::CObjectStateBase	(CInventoryItem *inventory_item, const u32 weapon_state, bool equality) : inherited("")
{
	Init							(inventory_item,weapon_state,equality);
}

CObjectStateBase::~CObjectStateBase	()
{
}

void CObjectStateBase::Init			(CInventoryItem *inventory_item, const u32 weapon_state, bool equality)
{
	m_inventory_item		= inventory_item;
	m_weapon_state			= weapon_state;
	m_equality				= equality;
}

void CObjectStateBase::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CObjectStateBase::reinit		(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CObjectStateBase::reload		(LPCSTR section)
{
	inherited::reload				(section);
}

void CObjectStateBase::initialize	()
{
	inherited::initialize			();
	
	m_started						= false;

	CWeapon							*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	if (weapon && (weapon->STATE == m_weapon_state) && !m_equality)
		m_started					= true;

	CMissile						*missile = dynamic_cast<CMissile*>(m_inventory_item);
	if (missile && (missile->State() == m_weapon_state))
		m_started					= true;
}

void CObjectStateBase::execute		()
{
	CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	if (weapon && (weapon->STATE == m_weapon_state) && !m_equality)
		m_started					= true;
	
	CMissile						*missile = dynamic_cast<CMissile*>(m_inventory_item);
	if (missile && (missile->State() == m_weapon_state))
		m_started					= true;
}

void CObjectStateBase::finalize		()
{
	inherited::finalize				();
}

bool CObjectStateBase::completed	() const
{
	CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	CMissile		*missile = dynamic_cast<CMissile*>(m_inventory_item);
	if (!weapon && !missile)
		return		(inherited::completed());

	if (weapon)
		return					(
			inherited::completed() &&
			(
			((weapon->STATE == m_weapon_state) && m_equality) ||
			((weapon->STATE != m_weapon_state) && !m_equality && m_started)
			)
		);
	else
		return					(
			inherited::completed() &&
			(
			((missile->State() == m_weapon_state) && m_equality) ||
			((missile->State() != m_weapon_state) && !m_equality && m_started)
			)
		);
}
