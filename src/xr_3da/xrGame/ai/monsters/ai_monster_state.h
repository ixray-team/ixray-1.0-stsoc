////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_state.h
//	Created 	: 06.07.2003
//  Modified 	: 01.10.2003
//	Author		: Serge Zhem
//	Description : interface for monster states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "ai_monster_defs.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class IState {

	enum {
		STATE_NOT_ACTIVE,
		STATE_INIT,
		STATE_RUN,
		STATE_DONE
	} m_tState;											

protected:
	enum {
		PRIORITY_NONE	 = 0,
		PRIORITY_LOWEST	 = 5,
		PRIORITY_LOW	 = 10,
		PRIORITY_NORMAL	 = 15,
		PRIORITY_HIGH	 = 20,
		PRIORITY_HIGHEST = 25,
	};
	
	
	TTime			m_dwCurrentTime;					//!< текущее время
	TTime			m_dwStateStartedTime;				//!< время перехода в это состояние
	TTime			m_dwNextThink;						//!< время следующего выполнения состояния
	TTime			m_dwTimeLocked;						//!< время заблокировки состояния
	
	TTime			m_dwInertia;						//!< инерция
	u8				m_Priority;							//!< приоритет состояния

public:
						IState			();
		virtual			~IState			() {}

				void	Execute			(TTime cur_time);						//!< bNothingChanged - true, если не было измений после предыдущего вызова данного метода

	IC	virtual	bool	CheckCompletion		() {return false;}						//!< возвращает true, если задание выполнено
	IC	virtual	bool	CheckStartCondition	() {return true;}						//!< возвращает true, если задание выполнено
	IC	virtual bool	IsCompleted			() {return false;}
	IC	virtual void	CriticalInterrupt	() {}
		virtual void	UpdateExternal		() {}

		virtual void	Reset			();										//!< инициализирует все значения по-умолчанию 

	IC			void	Activate		() {m_tState = STATE_INIT;}				//!< Перейти в данное состояние
	IC			bool	Active			() {return (STATE_NOT_ACTIVE != m_tState);}

		virtual void	Init			();									//!< Стадия инициализации состояния
		virtual void	Run				() = 0;								//!< Стадия выполнения состояния
		virtual void	Done			();									//!< Стадия завершения выполнения состояния

	IC			void	SetNextThink	(TTime next_think) {m_dwNextThink = next_think + m_dwCurrentTime;}


	/* определение приоритета состояния (реализация инерции) */ 
				void	SetPriority		(u8 new_priority) {m_Priority = new_priority;}

	IC			void	SetInertia		(TTime inertia) {m_dwInertia = inertia + m_dwCurrentTime;}
	IC			bool 	IsInertia		() {return ((PRIORITY_NONE != m_Priority) && (m_dwInertia > m_dwCurrentTime));}
	
	IC			u8		GetPriority		() {return m_Priority;}

	IC	virtual bool	GetStateAggressiveness	(){return false;}
};





