#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterThreatenThreatenAbstract CStateMonsterThreatenThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenThreatenAbstract::CStateMonsterThreatenThreaten(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenThreatenAbstract::~CStateMonsterThreatenThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::execute()
{
	m_object->MotionMan.m_tAction	= ACT_STAND_IDLE;

	// Смотреть на врага 
	m_object->DirMan.face_target(m_object->EnemyMan.get_enemy(), 1200);

	m_object->MotionMan.SetSpecParams	(ASP_THREATEN);
	m_object->sound().play		(MonsterSpace::eMonsterSoundThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::finalize()
{
	inherited::finalize();
}

