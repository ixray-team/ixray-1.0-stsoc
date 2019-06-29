////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_personal.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife personal event class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"

class NET_Packet;

class CALifeEventPersonal : public IPureSerializeObject<IReader,IWriter> {
public:
	ALife::_EVENT_ID			m_tEventID;
	ALife::_TIME_ID				m_tTimeID;
	ALife::_TASK_ID				m_tTaskID;
	int							m_iHealth;
	ALife::EEventRelationType	m_tEventRelationType;
	ALife::OBJECT_VECTOR		m_tpItemIDs;

protected:
	template <typename T>
	IC		void				load_data				(T &stream);
	template <typename T>
	IC		void				save_data				(T &stream);

public:
	virtual						~CALifeEventPersonal	();
	virtual void				load					(IReader &stream);
	virtual void				save					(IWriter &stream);
	virtual void				load					(NET_Packet &packet);
	virtual void				save					(NET_Packet &packet);
};

#include "alife_event_personal_inline.h"