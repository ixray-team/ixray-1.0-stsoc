#include "stdafx.h"

#include "WeaponHPSA.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponHPSA::CWeaponHPSA() : CWeaponPistol("HPSA")
{
}

CWeaponHPSA::~CWeaponHPSA()
{
}

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponHPSA::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponHPSA,CGameObject>("CWeaponHPSA")
			.def(constructor<>())
	];
}
