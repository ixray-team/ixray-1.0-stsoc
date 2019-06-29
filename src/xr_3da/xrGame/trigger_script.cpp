#include "stdafx.h"
/*
#include "trigger.h"
#include "script_space.h"
#include "net_utils.h"
#include "script_export_macroses.h"

using namespace luabind;

template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;

		DEFINE_LUA_WRAPPER_METHOD_V2(OnStateChanged,u32,u32)
		DEFINE_LUA_WRAPPER_METHOD_V1(Load, LPCSTR)
		DEFINE_LUA_WRAPPER_METHOD_R2P1_V1(net_Export, NET_Packet)
		DEFINE_LUA_WRAPPER_METHOD_R2P1_V1(net_Import, NET_Packet)
};

void CTrigger::script_register(lua_State *L)
{
	typedef CWrapperBase<CTrigger> WrapType;
	typedef CTrigger BaseType;
	module(L)
		[
			class_<CTrigger,CGameObject>("CTrigger")
			.def(										constructor<>()		)
			.def("GetState",							&BaseType::GetState)
			.def("SetState",							&BaseType::SetState)
			.def("PlayAnim",							&BaseType::PlayAnim)

			.def("OnStateChanged",						&BaseType::OnStateChanged, &WrapType::OnStateChanged_static)
			.def("Load",								&BaseType::Load, &WrapType::Load_static)

			.def("net_Export",							&BaseType::Load, &WrapType::net_Export_static)
			.def("net_Import",							&BaseType::net_Import, &WrapType::net_Import_static)

		];
}
*/