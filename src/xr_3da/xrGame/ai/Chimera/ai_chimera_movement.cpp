#include "stdafx.h"
#include "ai_chimera_movement.h"


CChimeraMovementManager::CChimeraMovementManager	()
{
	Init							();
}

CChimeraMovementManager::~CChimeraMovementManager	()
{
}

void CChimeraMovementManager::Init					()
{
}

void CChimeraMovementManager::Load	(LPCSTR section)
{
	inherited::Load					(section);
	
	inherited_shared::load_shared	(SUB_CLS_ID, section);

	
	m_movement_params.insert(std::make_pair(eVelocityParameterStand,		STravelParams(_sd->m_fsVelocityStandTurn.velocity.linear,		_sd->m_fsVelocityStandTurn.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalk,			STravelParams(_sd->m_fsVelocityWalkFwdNormal.velocity.linear,	_sd->m_fsVelocityWalkFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRun,			STravelParams(_sd->m_fsVelocityRunFwdNormal.velocity.linear,	_sd->m_fsVelocityRunFwdNormal.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterWalkDamaged,	STravelParams(_sd->m_fsVelocityWalkFwdDamaged.velocity.linear,	_sd->m_fsVelocityWalkFwdDamaged.velocity.angular)));
	m_movement_params.insert(std::make_pair(eVelocityParameterRunDamaged,	STravelParams(_sd->m_fsVelocityRunFwdDamaged.velocity.linear,	_sd->m_fsVelocityRunFwdDamaged.velocity.angular)));
}

void CChimeraMovementManager::reinit()
{
	inherited::reinit				();
}

void CChimeraMovementManager::reload(LPCSTR section)
{
	inherited::reload				(section);
}

void CChimeraMovementManager::load_shared(LPCSTR section)
{
	_sd->m_fsVelocityStandTurn.Load		(section,"Velocity_Stand");
	_sd->m_fsVelocityWalkFwdNormal.Load (section,"Velocity_WalkFwdNormal");
	_sd->m_fsVelocityWalkFwdDamaged.Load(section,"Velocity_WalkFwdDamaged");
	_sd->m_fsVelocityRunFwdNormal.Load	(section,"Velocity_RunFwdNormal");
	_sd->m_fsVelocityRunFwdDamaged.Load (section,"Velocity_RunFwdDamaged");
	_sd->m_fsVelocityDrag.Load			(section,"Velocity_Drag");
	_sd->m_fsVelocitySteal.Load			(section,"Velocity_Steal");

}


void CChimeraMovementManager::update()
{
	CLevelLocationSelector::set_evaluator	(0);
	CDetailPathManager::set_path_type		(eDetailPathTypeSmooth);
	set_path_type							(CMovementManager::ePathTypeGamePath);
	set_selection_type						(CMovementManager::eSelectionTypeRandomBranching);
	
	set_velocity_mask						(eVelocityParamsRun);	
	set_desirable_mask						(eVelocityParamsWalk);

	CDetailPathManager::set_try_min_time	(false); 
	enable_movement							(true);

	update_path								();
}

