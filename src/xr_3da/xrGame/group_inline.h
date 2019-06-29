////////////////////////////////////////////////////////////////////////////
//	Module 		: group_inline.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Group class inline functions
////////////////////////////////////////////////////////////////////////////

IC	CAgentManager &CGroup::agent_manager	() const
{
	VERIFY			(m_agent_manager);
	return			(*m_agent_manager);
}

IC	CAgentManager *CGroup::get_agent_manager	() const
{
	return			(m_agent_manager);
}
