////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_guard_planner.h
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker guard planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerGuardPlanner : public CActionPlannerActionScript<CAI_Stalker> {
private:
	typedef CActionPlannerActionScript<CAI_Stalker> inherited;

protected:
			void		add_evaluators			();
			void		add_actions				();

public:
						CStalkerGuardPlanner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual				~CStalkerGuardPlanner	();
	virtual	void		setup					(CAI_Stalker *object, CPropertyStorage *storage);
	virtual void		initialize				();
	virtual void		execute					();
	virtual void		finalize				();
};

#include "stalker_guard_planner_inline.h"