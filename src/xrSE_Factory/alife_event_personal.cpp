////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_personal.cpp
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife personal event class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "net_utils.h"
#pragma hdrstop

#include "alife_event_personal.h"

CALifeEventPersonal::~CALifeEventPersonal	()
{
}

template <typename T>
IC	void CALifeEventPersonal::load_data	(T &stream)
{
	stream.r					(&m_tEventID,		sizeof(m_tEventID));
	stream.r					(&m_tTimeID,		sizeof(m_tTimeID));
	stream.r					(&m_tTaskID,		sizeof(m_tTaskID));
	stream.r					(&m_iHealth,		sizeof(m_iHealth));
	stream.r					(&m_tEventRelationType,sizeof(m_tEventRelationType));
}

template <typename T>
IC	void CALifeEventPersonal::save_data	(T &stream)
{
	stream.w					(&m_tEventID,		sizeof(m_tEventID));
	stream.w					(&m_tTimeID,		sizeof(m_tTimeID));
	stream.w					(&m_tTaskID,		sizeof(m_tTaskID));
	stream.w					(&m_iHealth,		sizeof(m_iHealth));
	stream.w					(&m_tEventRelationType,sizeof(m_tEventRelationType));
}

void CALifeEventPersonal::load				(IReader &stream)
{
	load_data					(stream);
}

void CALifeEventPersonal::save				(IWriter &stream)
{
	save_data					(stream);
}

void CALifeEventPersonal::load				(NET_Packet &packet)
{
	load_data					(packet);
}

void CALifeEventPersonal::save				(NET_Packet &packet)
{
	save_data					(packet);
}
