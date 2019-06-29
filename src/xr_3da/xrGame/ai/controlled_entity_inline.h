#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CControlledEntityAbstract CControlledEntity<_Object>

TEMPLATE_SPECIALIZATION
CControlledEntityAbstract::CControlledEntity()
{
	m_controlled	= false; 
	m_data.m_object = 0;
	m_object		= 0;
}

TEMPLATE_SPECIALIZATION
CControlledEntityAbstract::~CControlledEntity()
{
}

TEMPLATE_SPECIALIZATION
void CControlledEntityAbstract::set_under_control(CController *controller)
{
	saved_id.team_id	= m_object->g_Team	();
	saved_id.squad_id	= m_object->g_Squad	();
	saved_id.group_id	= m_object->g_Group	();

	m_object->ChangeTeam(controller->g_Team(), controller->g_Squad(), controller->g_Group());

	m_controlled = true;
}

TEMPLATE_SPECIALIZATION
void CControlledEntityAbstract::free_from_control(CController *controller)
{
	m_object->ChangeTeam(saved_id.team_id, saved_id.squad_id, saved_id.group_id);
	m_controlled = false;
}	

TEMPLATE_SPECIALIZATION
void CControlledEntityAbstract::set_task_follow(const CEntity *e)
{
	m_data.m_object = e;
	m_data.m_task	= eTaskFollow;
}
TEMPLATE_SPECIALIZATION
void CControlledEntityAbstract::set_task_attack(const CEntity *e)
{
	m_data.m_object = e;
	m_data.m_task	= eTaskAttack;
}

TEMPLATE_SPECIALIZATION
void CControlledEntityAbstract::update()
{
	if (!m_controlled) return;
	if (m_data.m_object && m_data.m_object->getDestroy() && (m_data.m_task != eTaskNone)) {
		m_data.m_object = 0;
		m_data.m_task	= eTaskNone;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CControlledEntityAbstract