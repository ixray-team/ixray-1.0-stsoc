////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_no_alife.h"
#include "ai/stalker/ai_stalker.h"
#include "state_free_no_alife.h"
#include "state_gather_items.h"
#include "state_anomaly_check.h"
#include "state_hidden_enemy_check.h"
#include "state_back_enemy_check.h"
#include "state_watch_over.h"

CStateManagerNoALife::CStateManagerNoALife	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerNoALife::~CStateManagerNoALife	()
{
}

void CStateManagerNoALife::Init				()
{
}

void CStateManagerNoALife::Load				(LPCSTR section)
{
	add_state				(xr_new<CStateFreeNoAlife>		("FreeNoALife"),		eNoALifeStateFree,			1);
	add_state				(xr_new<CStateGatherItems>		("GatherItems"),		eNoALifeGatherItems,		0);
	add_state				(xr_new<CStateAnomalyCheck>		("AnomalyCheck"),		eNoALifeAnomalyCheck,		1);
	add_state				(xr_new<CStateHiddenEnemyCheck>	("HiddenEnemyCheck"),	eNoALifeHiddenEnemyCheck,	1);
	add_state				(xr_new<CStateBackEnemyCheck>	("BackEnemyCheck"),		eNoALifeBackEnemyCheck,		1);
	add_state				(xr_new<CStateWatchOver>		("WatchOver"),			eNoALifeWatchOver,			1);

	add_transition			(eNoALifeStateFree,eNoALifeGatherItems,				1,1);
	add_transition			(eNoALifeStateFree,eNoALifeAnomalyCheck,			1,1);
	add_transition			(eNoALifeStateFree,eNoALifeHiddenEnemyCheck,		1,1);
	add_transition			(eNoALifeStateFree,eNoALifeBackEnemyCheck,			1,1);
	add_transition			(eNoALifeStateFree,eNoALifeWatchOver,				1,1);
	add_transition			(eNoALifeGatherItems,eNoALifeAnomalyCheck,			1,1);
	add_transition			(eNoALifeGatherItems,eNoALifeHiddenEnemyCheck,		1,1);
	add_transition			(eNoALifeGatherItems,eNoALifeBackEnemyCheck,		1,1);
	add_transition			(eNoALifeGatherItems,eNoALifeWatchOver,				1,1);
	add_transition			(eNoALifeAnomalyCheck,eNoALifeHiddenEnemyCheck,		1,1);
	add_transition			(eNoALifeAnomalyCheck,eNoALifeBackEnemyCheck,		1,1);
	add_transition			(eNoALifeAnomalyCheck,eNoALifeWatchOver,			1,1);
	add_transition			(eNoALifeHiddenEnemyCheck,eNoALifeBackEnemyCheck,	1,1);
	add_transition			(eNoALifeHiddenEnemyCheck,eNoALifeWatchOver,		1,1);
	add_transition			(eNoALifeBackEnemyCheck,eNoALifeWatchOver,			1,1);

	inherited::Load			(section);
	
	m_free_probability		= .50f;
	m_watch_probability		= .00f;
	m_anomaly_probability	= .50f;
	m_hidden_probability	= .00f;
	m_back_probability		= .00f;
}

void CStateManagerNoALife::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object);
	set_current_state		(eNoALifeStateFree);
	set_dest_state			(eNoALifeStateFree);
}

void CStateManagerNoALife::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerNoALife::initialize		()
{
	inherited::initialize	();
}

void CStateManagerNoALife::choose_state		()
{
//	if (m_object->item()) {
//		set_dest_state		(eNoALifeGatherItems);
//		return;
//	}
//	else
		if (!current_state().completed() || (current_state_id() != dest_state_id()))
			return;

	float			rand_value = ::Random.randF(0,1.f);

	if (rand_value < m_watch_probability) {
		set_dest_state	(eNoALifeWatchOver);
		return;
	}
	
	rand_value		-= m_watch_probability;
	if (rand_value < m_anomaly_probability) {
		set_dest_state	(eNoALifeAnomalyCheck);
		return;
	}
	
	rand_value		-= m_anomaly_probability;
	if (rand_value < m_hidden_probability) {
		set_dest_state	(eNoALifeHiddenEnemyCheck);
		return;
	}
	
	rand_value		-= m_hidden_probability;
	if (rand_value < m_back_probability)
		set_dest_state	(eNoALifeBackEnemyCheck);
	else
		set_dest_state	(eNoALifeStateFree);
}

void CStateManagerNoALife::execute			()
{
	choose_state			();
	inherited::execute		();
}

void CStateManagerNoALife::finalize			()
{
	inherited::finalize		();
}