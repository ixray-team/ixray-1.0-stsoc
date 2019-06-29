////////////////////////////////////////////////////////////////////////////
//	Module 		: weapon_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Weapon manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CWeaponManager::firing		() const
{
	return					(m_bFiring);
}
