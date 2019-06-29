////////////////////////////////////////////////////////////////////////////
//	Module 		: state_death_recently_dead.cpp
//	Created 	: 15.01.2004
//  Modified 	: 15.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Death state (recently dead)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_death_recently_dead.h"

CStateDeathRecentlyDead::CStateDeathRecentlyDead	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateDeathRecentlyDead::~CStateDeathRecentlyDead	()
{
}

void CStateDeathRecentlyDead::Init			()
{
}

void CStateDeathRecentlyDead::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateDeathRecentlyDead::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateDeathRecentlyDead::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateDeathRecentlyDead::initialize		()
{
	inherited::initialize			();
}

void CStateDeathRecentlyDead::execute			()
{
}

void CStateDeathRecentlyDead::finalize		()
{
	inherited::finalize				();
}
