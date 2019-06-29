#include "stdafx.h"
#include "flesh.h"
#include "../../../script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CAI_Flesh::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_Flesh,CGameObject>("CAI_Flesh")
			.def(constructor<>())
	];
}
