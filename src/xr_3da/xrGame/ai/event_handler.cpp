#include "stdafx.h"
#include "event_handler.h"

void CListenerHolder::register_listener(IEventListener *listener)
{
	m_listeners.push_back(listener);
}

void CListenerHolder::remove_listener(IEventListener *listener)
{
	for (u32 i = 0; i<m_listeners.size();i++) {
		if (listener == m_listeners[i]) {
			m_listeners[i] = m_listeners.back();
			m_listeners.pop_back();
		}
	}
}

void CListenerHolder::process_event(const CEvent *event)
{
	for (u32 i = 0; i<m_listeners.size();i++) {
		if (m_listeners[i]->proper_event(event)) 
			m_listeners[i]->process_event(event);
	}
}


