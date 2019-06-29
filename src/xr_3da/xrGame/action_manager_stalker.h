////////////////////////////////////////////////////////////////////////////
//	Module 		: action_manager_stalker.h
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner.h"

class CAI_Stalker;

class CActionManagerStalker : public CActionPlanner<CAI_Stalker> {
public:
	enum EWorldProperties {
		eWorldPropertyAlive			= u32(0),
		eWorldPropertyDead,
		eWorldPropertyAlreadyDead,
		
		eWorldPropertyALife,
		eWorldPropertyMazeSolved,

		eWorldPropertyScript,
		eWorldPropertyDummy			= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorAlreadyDead	= u32(0),
		eWorldOperatorDead,
		eWorldOperatorFreeNoALife,
		eWorldOperatorFreeALife,
		eWorldOperatorScript,
		eWorldOperatorDummy			= u32(-1),
	};

protected:
	typedef CActionPlanner<CAI_Stalker>				inherited;
	typedef CGraphEngine::_solver_value_type		_value_type;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::CSolverConditionStorage	CConditionStorage;
	typedef CActionBase<CAI_Stalker>				CAction;

private:
	CConditionStorage		m_storage;

public:
							CActionManagerStalker	();
	virtual					~CActionManagerStalker	();
			void			init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					(CAI_Stalker *object);
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					(u32 time_delta);
			void			add_evaluators			();
			void			add_actions				();
};

#include "action_manager_stalker_inline.h"