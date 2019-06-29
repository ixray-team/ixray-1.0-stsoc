////////////////////////////////////////////////////////////////////////////
//	Module 		: greeting_manager.cpp
//	Created 	: 09.12.2004
//  Modified 	: 09.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Greeting manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "greeting_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "profiler.h"

const u32 FORGET_GREETING_TIME = 300000;

struct CRemoveOldGreetingPredicate {
	IC	bool	operator()	(const CGreetingManager::COldGreeting &greeting) const
	{
		return					(Device.dwTimeGlobal > greeting.m_time + FORGET_GREETING_TIME);
	}
};

IC	bool CGreetingManager::COldGreeting::operator==(const CAI_Stalker *stalker) const
{
	return						(m_object->ID() < stalker->ID());
}

CGreetingManager::CGreetingManager	(CCustomMonster *object)
{
	VERIFY						(object);
	m_object					= smart_cast<CAI_Stalker*>(object);
}

void CGreetingManager::reload		(LPCSTR section)
{
	m_old_greetings.clear		();
}

bool CGreetingManager::is_useful	(const CAI_Stalker *object) const
{
	return						(m_object->useful(this,object));
}

bool CGreetingManager::useful		(const CAI_Stalker *object) const
{
	return						(object->g_Alive() && !old_greeting(object));
}

float CGreetingManager::do_evaluate	(const CAI_Stalker *object) const
{
	return					(m_object->evaluate(this,object));
}

float CGreetingManager::evaluate	(const CAI_Stalker *object) const
{
	return						(m_object->Position().distance_to_sqr(object->Position()));
}

IC	void CGreetingManager::remove_old_greetings	()
{
	OLD_GREETINGS::iterator		I = remove_if(m_old_greetings.begin(),m_old_greetings.end(),CRemoveOldGreetingPredicate());
	m_old_greetings.erase		(I,m_old_greetings.end());
}

IC	const CGreetingManager::COldGreeting *CGreetingManager::old_greeting	(const CAI_Stalker *stalker) const
{
	OLD_GREETINGS::const_iterator	I = std::find(m_old_greetings.begin(),m_old_greetings.end(),stalker);
	if (I == m_old_greetings.end())
		return					(0);
	return						(&*I);
}

void CGreetingManager::update		()
{
	START_PROFILE("AI/Memory Manager/greetings/update")

	if (!m_object)
		return;

	remove_old_greetings		();

	inherited::update			();
	
	STOP_PROFILE
}

void CGreetingManager::process_greeting	()
{
	VERIFY						(selected());
	VERIFY						(!old_greeting(selected()));
	m_old_greetings.push_back	(COldGreeting(selected(),Device.dwTimeGlobal));
}
