////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeEventRegistry::CALifeEventRegistry			(LPCSTR section)
{
	m_id							= 0;
}

IC	const ALife::EVENT_MAP &CALifeEventRegistry::events	() const
{
	return							(m_events);
}

IC	CALifeEvent *CALifeEventRegistry::event				(const ALife::_EVENT_ID &event_id, bool no_assert) const
{
	ALife::EVENT_MAP::const_iterator	I = events().find(event_id);
	if (events().end() == I) {
		THROW2						(no_assert,"Specified event hasn't been found in the Event registry!");
		return						(0);
	}
	return							((*I).second);
}

IC	void CALifeEventRegistry::add						(CALifeEvent *event)
{
	if (m_events.find(event->m_tEventID) != m_events.end()) {
		THROW2						((*(m_events.find(event->m_tEventID))).second == event,"The specified event is already presented in the Event Registry!");
		THROW2						((*(m_events.find(event->m_tEventID))).second != event,"Event with the specified ID is already presented in the Event Registry!");
	}
	m_events.insert					(mk_pair(event->m_tEventID = m_id++,event));
}

IC	void CALifeEventRegistry::remove					(const ALife::_EVENT_ID &event_id, bool no_assert)
{
	ALife::EVENT_MAP::iterator		I = m_events.find(event_id);
	if (m_events.end() == I) {
		THROW2						(no_assert,"The specified event is not found in the Event Registry!");
		return;
	}
	m_events.erase					(I);
}

template <typename _predicate>
void CALifeEventRegistry::load							(IReader &file_stream, const _predicate &predicate)
{
	Msg							("* Loading events...");
	R_ASSERT2					(file_stream.find_chunk(EVENT_CHUNK_DATA),"Can't find chunk EVENT_CHUNK_DATA!");
	file_stream.r				(&m_id,sizeof(m_id));
	load_data					(m_events,file_stream,CEventLoader<_predicate>(predicate));
}
