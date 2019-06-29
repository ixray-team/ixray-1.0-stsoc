#include "stdafx.h"
#include "ai_biting.h"
#include "biting_state_manager.h"
#include "biting_state_panic.h"

CStateManagerBiting::CStateManagerBiting	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerBiting::~CStateManagerBiting	()
{
}

void CStateManagerBiting::Init				()
{
}

void CStateManagerBiting::Load				(LPCSTR section)
{
//	add_state				(xr_new<CStateBitingRest>("Rest Manager"),		eStateRest,		0);
//	add_state				(xr_new<CStateBitingPanic>("Panic Manager"),	eStatePanic,	1);
//	add_state				(xr_new<CStateBitingAttack>("Attack Manager"),	eStateAttack,	0);	
//
//	add_transition			(eStateRest,	eStatePanic,	1, 1);
//	add_transition			(eStateRest,	eStateAttack,	1, 1);
//	add_transition			(eStatePanic,	eStateAttack,	1, 1);

	inherited::Load			(section);
}

void CStateManagerBiting::reinit			(CAI_Biting *object)
{
	inherited::reinit		(object);

//	set_current_state		(eStateRest);
//	set_dest_state			(eStateRest);
}

void CStateManagerBiting::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerBiting::initialize		()
{
	inherited::initialize	();
}

void CStateManagerBiting::execute			()
{
//	(m_object->m_tEnemy.obj) ? set_dest_state(eStateAttack) : set_dest_state(eStateRest);

	inherited::execute		();
}

void CStateManagerBiting::finalize			()
{
	inherited::finalize		();
}

