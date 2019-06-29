#include "stdafx.h"
#include "WeaponAK74.h"
#include "WeaponHUD.h"

CWeaponAK74::CWeaponAK74(LPCSTR name, ESoundTypes eSoundType) : CWeaponMagazinedWGrenade(name, eSoundType)
{
/*
	Msg("sizeof %s = %d","CWeaponAmmo", sizeof(CWeaponAmmo));
	Msg("sizeof %s = %d","CInventoryItemObject", sizeof(CInventoryItemObject));
	Msg("sizeof %s = %d","CInventoryItem", sizeof(CInventoryItem));
	Msg("sizeof %s = %d","CHitImmunity", sizeof(CHitImmunity));
	Msg("sizeof %s = %d","CPhysicItem", sizeof(CPhysicItem));
	Msg("sizeof %s = %d","CPhysicsShellHolder", sizeof(CPhysicsShellHolder));

	Msg("sizeof %s = %d","CGameObject", sizeof(CGameObject));
	Msg("sizeof %s = %d","CParticlesPlayer", sizeof(CParticlesPlayer));
*/
}

CWeaponAK74::~CWeaponAK74()
{
}

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponAK74::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponAK74,CGameObject>("CWeaponAK74")
			.def(constructor<>())
	];
}
