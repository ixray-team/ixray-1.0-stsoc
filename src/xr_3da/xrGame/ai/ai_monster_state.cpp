////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_state.cpp
//	Created 	: 06.07.2003
//  Modified 	: 06.07.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_monster_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
IState::IState() 
{
	m_Priority			= PRIORITY_NONE;
	Reset				();
	
	m_dwCurrentTime		= 0;
}

void IState::Reset()
{
	
	m_dwNextThink		= 0;
	m_dwTimeLocked		= 0;
	m_tState			= STATE_NOT_ACTIVE;	

	m_dwInertia			= 0;
}


void IState::Execute(TTime cur_time) 
{
	m_dwCurrentTime = cur_time;

	switch (m_tState) {
		case STATE_NOT_ACTIVE:
		case STATE_INIT:
			Init();
		case STATE_RUN:
			if (!CheckCompletion()) {
				if (m_dwNextThink <= m_dwCurrentTime) {
					Run();
				}
				break;
			}
		case STATE_DONE:
			Done();
			break;
	}
}

void IState::Init()
{
	Reset					();

	m_dwNextThink			= m_dwCurrentTime;
	m_tState				= STATE_RUN;
	m_dwStateStartedTime	= m_dwCurrentTime;
}

void IState::Done()
{
	Reset();
}

