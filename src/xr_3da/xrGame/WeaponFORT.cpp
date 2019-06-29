#include "stdafx.h"

#include "WeaponFORT.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFORT::CWeaponFORT() : CWeaponPistol("FORT")
{
}

CWeaponFORT::~CWeaponFORT()
{
}

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponFORT::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponFORT,CGameObject>("CWeaponFORT")
			.def(constructor<>())
	];
}
