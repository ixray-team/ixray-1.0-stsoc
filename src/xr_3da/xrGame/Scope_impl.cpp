///////////////////////////////////////////////////////////////
// Scope.cpp
// Scope - апгрейд оружия снайперский прицел
///////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "scope.h"
//#include "PhysicsShell.h"

CScope::CScope	()
{
}

CScope::~CScope	() 
{
}


BOOL CScope::net_Spawn(CSE_Abstract* DC) 
{
	return	(inherited::net_Spawn(DC));
}

void CScope::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CScope::net_Destroy() 
{
	inherited::net_Destroy();
}

void CScope::UpdateCL() 
{
	inherited::UpdateCL();
}

void CScope::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CScope::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
}

void CScope::renderable_Render() 
{
	inherited::renderable_Render();
}

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CScope::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CScope,CGameObject>("CScope")
			.def(constructor<>())
	];
}
