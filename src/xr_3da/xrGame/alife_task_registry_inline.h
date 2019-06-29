////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeTaskRegistry::CALifeTaskRegistry	(LPCSTR section)
{
	m_id						= 0;
}

IC	void CALifeTaskRegistry::update			(CALifeTask *task)
{
	R_ASSERT2					(this->task(task->m_tTaskID) == task,"Cannot find a specified task in the Task registry!");
	ALife::OBJECT_TASK_PAIR_IT	J = m_cross.find(task->m_tCustomerID);
	if (m_cross.end() == J) {
		m_cross.insert			(mk_pair(task->m_tCustomerID,ALife::TASK_SET()));
		J						= m_cross.find(task->m_tCustomerID);
	}
	(*J).second.insert			(task->m_tTaskID);
}

IC	void CALifeTaskRegistry::add			(CALifeTask *task)
{
	m_tasks.insert				(mk_pair(task->m_tTaskID = m_id++,task));
	update						(task);
}

IC	void CALifeTaskRegistry::remove			(const ALife::_TASK_ID &id, bool no_assert)
{
	ALife::TASK_PAIR_IT			I = m_tasks.find(id);
	if (I == m_tasks.end()) {
		THROW2					(no_assert,"Cannot find a specified task in the Task registry!");
		return;
	}
	m_tasks.erase				(I);
	
//	ALife::OBJECT_TASK_PAIR_IT	i = m_cross.find(id);
//	VERIFY						(i != m_cross.end());
//	m_cross.erase				(i);
}

IC	const ALife::TASK_MAP &CALifeTaskRegistry::tasks() const
{
	return						(m_tasks);
}

IC	CALifeTask *CALifeTaskRegistry::task	(const ALife::_TASK_ID &id, bool no_assert) const
{
	ALife::TASK_MAP::const_iterator	I = tasks().find(id);
	if (I == tasks().end()) {
		THROW2					(no_assert,"Cannot find a specified task in the Task registry!");
		return					(0);
	}
	return						((*I).second);
}

IC	const ALife::OBJECT_TASK_MAP &CALifeTaskRegistry::cross	() const
{
	return						(m_cross);
}

IC	void CALifeTaskRegistry::clear	()
{
	delete_data					(m_tasks);
	m_cross.clear				();
	m_id						= 0;
}
