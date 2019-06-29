#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterThreatenAbstract CStateMonsterThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenAbstract::CStateMonsterThreaten(LPCSTR state_name, SSubStatePtr state_walk, SSubStatePtr state_threaten) : inherited(state_name)
{
	states[eStateWalk]			= state_walk;
	states[eStateThreaten]		= state_threaten;
}

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenAbstract::~CStateMonsterThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenAbstract::Load(LPCSTR section)
{
	add_state				(states[eStateWalk],		eStateWalk,			1);
	add_state				(states[eStateThreaten],	eStateThreaten,		0);

	add_transition			(eStateWalk,eStateThreaten,1,1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eStateWalk);
	set_dest_state			(eStateThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenAbstract::execute()
{
	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenAbstract::finalize()
{
	inherited::finalize();
}

