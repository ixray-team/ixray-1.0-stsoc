#include "stdafx.h"
#include "ai_biting.h"
#include "biting_state_attack.h"
#include "biting_state_attack_run.h"
#include "biting_state_attack_melee.h"

CStateBitingAttack::CStateBitingAttack(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingAttack::~CStateBitingAttack	()
{
}


void CStateBitingAttack::Load(LPCSTR section)
{
	add_state				(xr_new<CStateBitingAttackRun>		("Attack Run"),				eAS_Run,		1);
	add_state				(xr_new<CStateBitingAttackMelee>	("Attack Melee"),			eAS_Melee,		0);

	add_transition			(eAS_Run, eAS_Melee, 1,1);

	inherited::Load			(section);
}

void CStateBitingAttack::reinit(CAI_Biting *object)
{
	inherited::reinit		(object);
	set_current_state		(eAS_Run);
	set_dest_state			(eAS_Run);
}



void CStateBitingAttack::initialize()
{
	inherited::initialize();
}

void CStateBitingAttack::execute()
{
	float m_fDistMin = m_object->m_fCurMinAttackDist;
	float m_fDistMax = m_object->_sd->m_fMaxAttackDist - (m_object->_sd->m_fMinAttackDist - m_object->m_fCurMinAttackDist);

	const CEntity *enemy = m_object->m_tEnemy.obj;

	// определить расстояние до противника
	float dist = enemy->Position().distance_to(m_object->Position());
	
	// определить тип атаки
	bool b_melee = false;
	if ((current_state_id() == eAS_Melee) && (dist < m_fDistMax)) b_melee = true;
	else if (dist < m_fDistMin) b_melee = true;


	// установить целевое состояние
	if (b_melee)	set_dest_state(eAS_Melee);
	else			set_dest_state(eAS_Run);

	inherited::execute();
}

void CStateBitingAttack::finalize()
{
	inherited::finalize();
}

