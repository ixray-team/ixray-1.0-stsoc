#include "stdafx.h"
#include "UIProgressBar.h"

#include "../script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIProgressBar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIProgressBar, CUIWindow>("CUIProgressBar")
		.def(						constructor<>())

		.def("SetRange",				&CUIProgressBar::SetRange)
		.def("GetRange_min",			&CUIProgressBar::GetRange_min)
		.def("GetRange_max",			&CUIProgressBar::GetRange_max)
	];
}