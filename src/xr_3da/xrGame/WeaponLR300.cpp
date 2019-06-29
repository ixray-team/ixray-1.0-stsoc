#include "stdafx.h"
#include "WeaponLR300.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponLR300::CWeaponLR300		() : CWeaponMagazined("LR300",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
}

CWeaponLR300::~CWeaponLR300		()
{
}
/*
void	CWeaponLR300::UpdateCL	()
{
	inherited::UpdateCL	();
	Fvector C;
	Center	(C);
	Msg		("- V%c E%c REG%c SEC%c H%c TYPE:%d - %3.1f,%3.1f,%3.1f,%f", 
		getVisible()?'+':'-', 
		getEnabled()?'+':'-', 
		spatial.node_ptr?'+':'-',
		spatial.sector?'+':'-',
		H_Parent()?'+':'-',
		spatial.type,
		VPUSH(C),
		Radius()
		);
}

void	CWeaponLR300::renderable_Render		()
{
	Msg		("! CWeaponLR300::render");
	inherited::renderable_Render();
}

void	CWeaponLR300::spatial_move			()
{
	Msg		("! CWeaponLR300::s_move");
	inherited::spatial_move		();
}

void	CWeaponLR300::spatial_register		()
{
	Msg		("! CWeaponLR300::s_register");
	inherited::spatial_register	();
}

void	CWeaponLR300::spatial_unregister	()
{
	Msg		("! CWeaponLR300::s_unregister");
	inherited::spatial_unregister();
}
*/

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponLR300::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponLR300,CGameObject>("CWeaponLR300")
			.def(constructor<>())
	];
}
