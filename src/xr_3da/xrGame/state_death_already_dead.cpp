////////////////////////////////////////////////////////////////////////////
//	Module 		: state_death_already_dead.cpp
//	Created 	: 15.01.2004
//  Modified 	: 15.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Death state (already dead)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_death_already_dead.h"

CStateDeathAlreadyDead::CStateDeathAlreadyDead	(LPCSTR state_name) : inherited(state_name)
{
	Init							();
}

CStateDeathAlreadyDead::~CStateDeathAlreadyDead	()
{
}

void CStateDeathAlreadyDead::Init			()
{
}

void CStateDeathAlreadyDead::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CStateDeathAlreadyDead::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CStateDeathAlreadyDead::reload			(LPCSTR section)
{
	inherited::reload				(section);
}

void CStateDeathAlreadyDead::initialize		()
{
	inherited::initialize			();
}

void CStateDeathAlreadyDead::execute			()
{
}

void CStateDeathAlreadyDead::finalize			()
{
	inherited::finalize				();
}

