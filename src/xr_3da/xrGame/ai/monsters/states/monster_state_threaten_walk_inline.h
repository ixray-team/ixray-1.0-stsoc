#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterThreatenWalkAbstract CStateMonsterThreatenWalk<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenWalkAbstract::CStateMonsterThreatenWalk(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenWalkAbstract::~CStateMonsterThreatenWalk()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenWalkAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenWalkAbstract::execute()
{
	//m_object->MotionMan.m_tAction	= ACT_WALK_FWD;
	//m_object->MoveToTarget			(m_object->EnemyMan.get_enemy());
	//m_object->sound().play	(MonsterSpace::eMonsterSoundThreaten, 0,0,m_object->get_sd()->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenWalkAbstract::finalize()
{
	inherited::finalize();
}

