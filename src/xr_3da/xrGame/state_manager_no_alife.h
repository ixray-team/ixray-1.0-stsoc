////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CAI_Stalker;

class CStateManagerNoALife : public CStateManagerState<CAI_Stalker> {
protected:
	typedef CStateManagerState<CAI_Stalker> inherited;
	enum ENoALifeStates {
		eNoALifeStateFree = u32(0),
		eNoALifeGatherItems,
		eNoALifeAnomalyCheck,
		eNoALifeHiddenEnemyCheck,
		eNoALifeBackEnemyCheck,
		eNoALifeWatchOver,
		eNoALifeStateDummy = u32(-1),
	};

public:
						CStateManagerNoALife	(LPCSTR state_name);
	virtual				~CStateManagerNoALife	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
			void		choose_state			();

private:
	float				m_free_probability;
	float				m_watch_probability;
	float				m_anomaly_probability;
	float				m_hidden_probability;
	float				m_back_probability;
};

#include "state_manager_no_alife_inline.h"