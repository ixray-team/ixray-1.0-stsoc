
#include "stdafx.h"
#include "biting_state_panic.h"
#include "biting_state_panic_run.h"
#include "ai_biting.h"

CStateBitingPanic::CStateBitingPanic(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingPanic::~CStateBitingPanic	()
{
}

void CStateBitingPanic::Load(LPCSTR section)
{
	add_state				(xr_new<CStateBitingPanicRun>("Panic Run"), ePS_RunAway, 1);

	inherited::Load			(section);

}

void CStateBitingPanic::reinit(CAI_Biting *object)
{
	inherited::reinit		(object);
	set_current_state		(ePS_RunAway);
	set_dest_state			(ePS_RunAway);
}

void CStateBitingPanic::initialize()
{
	inherited::initialize();
}

void CStateBitingPanic::execute()
{
	// Add here
	set_dest_state(ePS_RunAway);

	inherited::execute();
}

void CStateBitingPanic::finalize()
{
	inherited::finalize();
}

