////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_abstract.cpp
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_manager_abstract.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename T\
>

#define CAbstractStateManager CStateManagerAbstract<T>

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
