#include "stdafx.h"

#include "script_ui_registrator.h"
//.#include "UI\UIInventoryWnd.h"
//. #include "UI\UIStatsWnd.h"
#include "UI\UIMultiTextStatic.h"
//.#include "UIDMFragList.h"
//.#include "UIDMPlayerList.h"
#include "script_space.h"
using namespace luabind;

#pragma optimize("s",on)
void UIRegistrator::script_register(lua_State *L)
{
	module(L)
	[

//.		class_<CUIStatsWnd, CUIDialogWnd>("CUIStatsWnd")
//.		.def("GetFrameRect",  &CUIStatsWnd::GetFrameRect),

//.		class_<CUIDMPlayerList, CUIDMFragList>("CUIDMPlayerList")
//.		.def(					constructor<>()),

//.		class_<CUIInventoryWnd, CUIDialogWnd>("CUIInventoryWnd")
//.		.def(					constructor<>()),

		class_<CGameFont>("CGameFont")
			.enum_("EAligment")
			[
				value("alLeft",						int(CGameFont::alLeft)),
				value("alRight",					int(CGameFont::alRight)),
				value("alCenter",					int(CGameFont::alCenter))
			],

		class_<CUICaption>("CUICaption")
			.def("addCustomMessage",	&CUICaption::addCustomMessage)
			.def("setCaption",			&CUICaption::setCaption)
	];
}
