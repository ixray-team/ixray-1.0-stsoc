
#include "stdafx.h"
#include "biting_state_rest.h"
#include "biting_state_rest_sleep.h"
#include "biting_state_rest_wander.h"
#include "ai_biting.h"

CStateBitingRest::CStateBitingRest(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingRest::~CStateBitingRest	()
{
}

void CStateBitingRest::Load(LPCSTR section)
{
	add_state				(xr_new<CStateBitingSleep>		("Sleep"),				eRS_Sleep,				1);
	add_state				(xr_new<CStateBitingWander>		("Wander"),				eRS_WalkGraphPoint,		0);

	add_transition			(eRS_Sleep,eRS_WalkGraphPoint,	1,1);

	inherited::Load			(section);
}

void CStateBitingRest::reinit(CAI_Biting *object)
{
	inherited::reinit		(object);
	set_current_state		(eRS_Sleep);
	set_dest_state			(eRS_Sleep);
}

void CStateBitingRest::initialize()
{
	inherited::finalize();
}

void CStateBitingRest::execute()
{
	bool bNormalSatiety =	(m_object->GetSatiety() > m_object->_sd->m_fMinSatiety) && 
							(m_object->GetSatiety() < m_object->_sd->m_fMaxSatiety); 

	if (bNormalSatiety) {
		set_dest_state	(eRS_Sleep);
	} else {
		set_dest_state	(eRS_WalkGraphPoint);
	}

	inherited::execute();
}

void CStateBitingRest::finalize()
{
	inherited::finalize();
}

