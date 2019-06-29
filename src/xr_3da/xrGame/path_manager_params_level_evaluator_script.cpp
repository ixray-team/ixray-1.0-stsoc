////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_params_level_evaluator.h
//	Created 	: 21.03.2002
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "path_manager_params_level_evaluator.h"
#include "script_space.h"

using namespace luabind;

void CAbstractVertexEvaluator::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAbstractVertexEvaluator>("vertex_evaluator")
	];
}
