////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries_inline.h
//	Created 	: 05.01.2002
//  Modified 	: 04.05.2004
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectRegistry
//////////////////////////////////////////////////////////////////////////

IC	CSE_ALifeDynamicObject *CSE_ALifeObjectRegistry::object	(ALife::_OBJECT_ID tObjectID, bool bNoAssert)
{
	ALife::D_OBJECT_PAIR_IT		I = m_tObjectRegistry.find(tObjectID);

	if (!bNoAssert) {
		R_ASSERT2				(m_tObjectRegistry.end() != I,"Specified object hasn't been found in the Object registry!");
	}
	else
		if (m_tObjectRegistry.end() == I)
			return				(0);

	return						((*I).second);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeEventRegistry
//////////////////////////////////////////////////////////////////////////

IC	CSE_ALifeEvent *CSE_ALifeEventRegistry::event	(ALife::_EVENT_ID tEventID, bool bNoAssert)
{
	ALife::EVENT_PAIR_IT		I = m_tEventRegistry.find(tEventID);

	if (!bNoAssert) {
		R_ASSERT2				(m_tEventRegistry.end() != I,"Specified event hasn't been found in the Event registry!");
	}
	else
		if (m_tEventRegistry.end() == I)
			return				(0);

	return						((*I).second);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeTaskRegistry
//////////////////////////////////////////////////////////////////////////

IC	CSE_ALifeTask *CSE_ALifeTaskRegistry::task	(ALife::_TASK_ID tTaskID, bool bNoAssert)
{
	ALife::TASK_PAIR_IT			I = m_tTaskRegistry.find(tTaskID);

	if (!bNoAssert) {
		R_ASSERT2				(m_tTaskRegistry.end() != I,"Specified task hasn't been found in the Task registry!");
	}
	else
		if (m_tTaskRegistry.end() == I)
			return				(0);

	return						((*I).second);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphRegistry
//////////////////////////////////////////////////////////////////////////

IC	void CSE_ALifeGraphRegistry::update_next	()
{
	VERIFY						(m_tpCurrentLevel);
	if (m_tpCurrentLevel->empty())
		m_next_iterator			= m_tpCurrentLevel->begin();
	else {
		++m_next_iterator;
		if (m_next_iterator == m_tpCurrentLevel->end())
			m_next_iterator		= m_tpCurrentLevel->begin();
	}
}

IC	CSE_ALifeGraphRegistry::_iterator &CSE_ALifeGraphRegistry::next		()
{
	return						(m_next_iterator);
}

IC	bool CSE_ALifeGraphRegistry::time_over		()
{
	return						(CPU::GetCycleCount() - m_start_time > m_max_process_time);
}

IC	void CSE_ALifeGraphRegistry::set_process_time(u64 process_time)
{
	m_max_process_time			= process_time;
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeScheduleRegistry
//////////////////////////////////////////////////////////////////////////

IC	CSE_ALifeScheduleRegistry::CSE_ALifeScheduleRegistry	()
{
	Init						();
}

IC	void CSE_ALifeScheduleRegistry::Init					()
{
	m_tpScheduledObjects.clear	();
	update_next					();
	m_cycle_count				= 0;
}

IC	void CSE_ALifeScheduleRegistry::update_next				()
{
	if (m_tpScheduledObjects.empty())
		m_next_iterator			= m_tpScheduledObjects.begin();
	else {
		++m_next_iterator;
		if (m_next_iterator == m_tpScheduledObjects.end())
			m_next_iterator		= m_tpScheduledObjects.begin();
	}
}

IC	CSE_ALifeScheduleRegistry::_iterator &CSE_ALifeScheduleRegistry::next				()
{
	return						(m_next_iterator);
}

IC	bool CSE_ALifeScheduleRegistry::time_over		()
{
	return						(CPU::GetCycleCount() - m_start_time > m_max_process_time);
}

IC	void CSE_ALifeScheduleRegistry::set_process_time(u64 process_time)
{
	m_max_process_time			= process_time;
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeNewsRegistry
//////////////////////////////////////////////////////////////////////////

IC	void CSE_ALifeNewsRegistry::add						(const ALife::SGameNews &news)
{
	ALife::SGameNews				*_news = xr_new<ALife::SGameNews>(news);
	_news->m_news_id				= m_last_id++;
	m_news.insert					(std::make_pair(m_last_id,_news));
}

IC	void CSE_ALifeNewsRegistry::remove					(const ALife::_NEWS_ID &news_id)
{
	NEWS_REGISTRY::iterator			I = m_news.find(news_id);
	VERIFY							(news().end() != I);
	m_news.erase					(I);
}

IC	const CSE_ALifeNewsRegistry::NEWS_REGISTRY &CSE_ALifeNewsRegistry::news	() const
{
	return							(m_news);
}

IC	const ALife::SGameNews *CSE_ALifeNewsRegistry::news		(const ALife::_NEWS_ID &news_id) const
{
	NEWS_REGISTRY::const_iterator	I = news().find(news_id);
	if (news().end() == I)
		return						(0);
	return							((*I).second);
}
