#include "stdafx.h"
#include "ai_monster_movement.h"
#include "biting/ai_biting.h"

//////////////////////////////////////////////////////////////////////////
// Init Movement
void CMonsterMovement::Update_Initialize()
{
	CLevelLocationSelector::set_evaluator			(0);
	CDetailPathManager::set_path_type				(eDetailPathTypeSmooth);
	b_try_min_time									= true;
	b_enable_movement								= true;
	set_path_targeted								(false);
	set_use_dest_orient								(false);

	MotionStats->update								();
}


//////////////////////////////////////////////////////////////////////////
// Update Movement
void CMonsterMovement::Update_Execute()
{
#ifdef _DEBUG	
	//update_target_point								();
#endif
	
	CDetailPathManager::set_try_min_time			(b_try_min_time); 
	CDetailPathManager::set_use_dest_orientation	(b_use_dest_orient);
	enable_movement									(b_enable_movement);
	update_path										();
}


//////////////////////////////////////////////////////////////////////////
// Finalize Movement
void CMonsterMovement::Update_Finalize()
{
	set_desirable_speed						(m_object->m_fCurSpeed);
}


//////////////////////////////////////////////////////////////////////////
// update path with new scheme method
void CMonsterMovement::update_target_point() 
{
	m_path_end	= false;
	m_failed	= false;

	/// проверить условия, когда путь строить не нужно
	if (!b_enable_movement)			return;
	if (!check_build_conditions())	return;

	Msg(" ---- Update [%u] ---- ", Level().timeServer());

	// получить промежуточные позицию и ноду
	STarget saved_target;
	saved_target = m_intermediate;

	get_intermediate		();
	// установить параметры
	set_parameters			();

	if (IsPathEnd(m_distance_to_path_end)) {
		if (saved_target.position.similar(m_intermediate.position) && (saved_target.node == m_intermediate.node)) {
			m_failed = true;
		}
	}

	m_last_time_path_update	= m_object->m_current_update;
}

