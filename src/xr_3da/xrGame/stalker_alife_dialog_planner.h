////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_dialog_planner.h
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife dialog planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerALifeDialogPlanner : public CActionPlannerActionScript<CAI_Stalker> {
private:
	typedef CActionPlannerActionScript<CAI_Stalker> inherited;

public:
						CStalkerALifeDialogPlanner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual				~CStalkerALifeDialogPlanner	();
	virtual	void		setup						(CAI_Stalker *object, CPropertyStorage *storage);
			void		add_evaluators				();
			void		add_actions					();
	virtual	void		initialize					();
};
