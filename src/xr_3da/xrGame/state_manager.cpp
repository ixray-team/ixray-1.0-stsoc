////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager.cpp
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "state_manager.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename T\
>

#define CAbstractStateManager CStateManager<T>

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
